#include "kernel.h"
#include "gfx/vga.h"
#include "io/uart.h"
#include "io/pic.h"
#include "io/io.h"
#include "io/pit.h"
#include "io/cpu.h"
#include "memory/gdt.h"
#include "memory/tss.h"
#include "memory/idt.h"
#include "memory/paging.h"
#include "interrupts/interrupts.h"
#include "interrupts/keyboard.h"
#include "interrupts/timer.h"
#include "multitask/taskSwitch.h"
#include "multitask/multitask.h"
#include "multitask/modules.h"
#include "multitask/elf.h"
#include "file/filesystem.h"
#include "stdlib.h"

extern uint32_t __tss_stack;
TSS tssEntry;
multiboot_info_t* pMultiboot;

extern "C" void kernel_main(multiboot_info_t* mbd) 
{
    pMultiboot = mbd;

    VGA_Init({  pMultiboot->framebuffer_width, pMultiboot->framebuffer_height, 
                pMultiboot->framebuffer_pitch, (uint32_t*)pMultiboot->framebuffer_addr });
    VGA_Clear();

    // Welcome message
    VGA_printf("-------------------------------------------------------------------------------", true, VGA_COLOUR_GREEN);
    VGA_printf("                                    MinOS                                      ", true, VGA_COLOUR_GREEN);
    VGA_printf("-------------------------------------------------------------------------------", true, VGA_COLOUR_GREEN);
    VGA_printf(" ");

    // Start COM1 serial port
    UART COM1 = UART(UART::COM1);
    UART::pCOM = &COM1;
    COM1.printf("MinOS running from COM1 at ", false);
    COM1.printf<uint16_t, true>((uint16_t)UART::COM1);
    VGA_printf("[Success] ", false, VGA_COLOUR_LIGHT_GREEN);
    VGA_printf("UART communication established on COM1 at ", false);
    VGA_printf<uint16_t, true>((uint16_t)COM1.m_Com);

    // Create TSS
    tssEntry = CreateTSSEntry((uint32_t)&__tss_stack, 0x10); // Stack pointer and ring 0 data selector 

    // Construct GDT entries (0xFFFFF actually translates to all of memory)
    GDTTable[0] = CreateGDTEntry(0, 0, 0);                                          // GDT entry at 0x0 cannot be used
    GDTTable[1] = CreateGDTEntry(0x00000000, 0xFFFFF, GDT_CODE_PL0);                // Code      - 0x8
    GDTTable[2] = CreateGDTEntry(0x00000000, 0xFFFFF, GDT_DATA_PL0);                // Data      - 0x10
    GDTTable[3] = CreateGDTEntry(0x00000000, 0xFFFFF, GDT_CODE_PL3);                // User code - 0x18
    GDTTable[4] = CreateGDTEntry(0x00000000, 0xFFFFF, GDT_DATA_PL3);                // User data - 0x20
    GDTTable[5] = CreateGDTEntry((uint32_t) &tssEntry, sizeof(tssEntry), TSS_PL0);  // TSS       - 0x28

    // Load GDT
    LoadGDT(GDTTable, sizeof(GDTTable));
    VGA_printf("[Success] ", false, VGA_COLOUR_LIGHT_GREEN);
    VGA_printf("GDT sucessfully loaded");
    VGA_printf("");
    PrintGDT(GDTTable, 6);
    VGA_printf("");

    // Load TSS
    LoadTSS(((uint32_t)&GDTTable[5] - (uint32_t)&GDTTable) | 0b11); // Set last 2 bits for RPL 3
    VGA_printf("[Success] ", false, VGA_COLOUR_LIGHT_GREEN);
    VGA_printf("TSS sucessfully loaded");

    // Read memory map from GRUB
    if ((mbd->flags & 6) == 0) {  VGA_printf("[Failure] Multiboot error!", true, VGA_COLOUR_LIGHT_RED); }

    // Map out memory
    uint32_t maxMemoryRange = GetMaxMemoryRange(pMultiboot);

    // Pging messes up GRUB modules
    MoveGrubModules(pMultiboot);

    // Page frame allocation
    InitPaging(maxMemoryRange);

    // Setup PIT
    InitPIT();
    
    // Init PIC, create IDT entries and enable interrupts
    InitInterrupts(PIC_MASK_PIT_AND_KEYBOARD, PIC_MASK_ALL);
    VGA_printf("[Success] ", false, VGA_COLOUR_LIGHT_GREEN);
    VGA_printf("IDT sucessfully loaded");

    // Setup keyboard driver
    KeyboardInit();

    // Test for SSE
    bool bSSE = IsSSESupported();
    if (bSSE)
    {
        VGA_printf("[Success] ", false, VGA_COLOUR_LIGHT_GREEN);
        VGA_printf("SSE2 supported");
        EnableSSE();
    }
    else
    {
        VGA_printf("[Failure] ", false, VGA_COLOUR_LIGHT_RED);
        VGA_printf("SSE not supported!");
    }

    // Load GRUB modules and build filesystem
    uint32_t vfsAddress = LoadGrubVFS(pMultiboot);
    BuildVFS(vfsAddress);

    // Load window manager
    FileHandle cli = kFileOpen("cli");
    void* cliBuffer = kmalloc(kGetFileSize(cli));
    kFileRead(cli, cliBuffer);
    auto elf = LoadElfFile(cliBuffer);
    CreateTask("cli", elf.entry, elf.size, elf.location);
    kfree(cliBuffer, kGetFileSize(cli));
    kFileClose(cli);

    VGA_printf("[Success] ", false, VGA_COLOUR_LIGHT_GREEN);
    VGA_printf("Loaded cli from ramdisk");
    
    VGA_printf("");
    PrintPaging();
    VGA_printf("");
    VGA_printf("Enabling scheduler and interrupts...");
    
    EnableScheduler();

    // Hang and wait for interrupts
    while (true) { asm("hlt"); }
}
