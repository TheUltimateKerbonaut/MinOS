#include "io/vga.h"
#include "io/serial.h"

extern "C" void kernel_main(void) 
{
    VGA_Clear();

    VGA_printf("--------------------------------------------------------------------------------");
    VGA_printf("                                      MinOS                                     ");
    VGA_printf("-------------------------------------------------------------------------------");

    VGA_printf("Welcome to MinOS!");

    VGA_DisableCursor();
    VGA_EnableCursor();
}