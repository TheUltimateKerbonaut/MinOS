#ifdef __cplusplus
#pragma once
#endif
#ifndef SYSCALLS_H
#define SYSCALLS_H

#include <stdint.h>
#include <stddef.h>

#define SYSCALL_ARGS_0(type, fn, num) type fn() { int a; asm volatile("int $0x80" : "=a" (a) : "0" (num)); return (type) a; }
#define SYSCALL_ARGS_1(type, fn, num, p1, n1) type fn(p1 n1) { int a; asm volatile("int $0x80" : "=a" (a) : "0" (num), "b" ((unsigned int)n1)); return (type) a; }
#define SYSCALL_ARGS_2(type, fn, num, p1, n1, p2, n2) type fn(p1 n1, p2 n2) { int a; asm volatile("int $0x80" : "=a" (a) : "0" (num), "b" ((unsigned int)n1), "c" ((unsigned int)n2)); return (type) a; }
#define SYSCALL_ARGS_3(type, fn, num, p1, n1, p2, n2, p3, n3) type fn(p1 n1, p2 n2, p3 n3) { int a; asm volatile("int $0x80" : "=a" (a) : "0" (num), "b" ((unsigned int)n1), "c" ((unsigned int)n2), "d" ((unsigned int)n3)); return (type) a; }

struct sMmapArgs
{
    void* addr;
    size_t length;
    int prot;
    int flags;
    int fd;
    long int offset;
};

#ifndef Kernel

void _exit(int status)
{
    asm volatile("mov $0, %eax; int $0x80");
}

SYSCALL_ARGS_1(int, close, 1, int, file)
SYSCALL_ARGS_3(int, execve, 2, const char*, name, char* const*, argv, char* const*, env)
SYSCALL_ARGS_0(int, fork, 3)
SYSCALL_ARGS_2(int, fstat, 4, int, file, struct stat*, st)
SYSCALL_ARGS_0(int, getpid, 5)
SYSCALL_ARGS_1(int, isatty, 6, int, fd)
SYSCALL_ARGS_2(int, kill, 7, int, pid, int, sig)
SYSCALL_ARGS_2(int, link, 8, const char*, old, const char*, new)
SYSCALL_ARGS_3(off_t, lseek, 9, int, file, off_t, ptr, int, dir)

#include <errno.h>
#undef errno
extern int errno;
int open(const char* name, int flags, ...)
{
    int a;
    asm volatile("int $0x80" : "=a" (a) : "0" (10));
    return a;
}

SYSCALL_ARGS_3(int, read, 11, int, file, void*, ptr, size_t, len)
SYSCALL_ARGS_1(void*, sbrk, 12, ptrdiff_t, incr)
SYSCALL_ARGS_2(int, stat, 13, const char*, file, struct stat*, st)
SYSCALL_ARGS_1(clock_t, times, 14, struct tms*, buf)
SYSCALL_ARGS_1(int, unlink, 15, const char*, name)
SYSCALL_ARGS_1(int, wait, 16, int*, status)
SYSCALL_ARGS_3(int, write, 17, int, file, const void*, ptr, size_t, len)
SYSCALL_ARGS_2(int, gettimeofday, 18, struct timeval*, p, void*, tz)

void* mmap(void* addr, size_t length, int prot, int flags, int fd, off_t offset)
{
    volatile struct sMmapArgs args;
    args.addr = addr;
    args.length = length;
    args.prot = prot;
    args.flags = flags;
    args.fd = fd;
    args.offset = offset;

    void* a;
    asm volatile("int $0x80" : "=a" (a) : "0" (19), "b" ((unsigned int)&args));
    return a;
}

SYSCALL_ARGS_2(int, munmap, 20, void*, addr, size_t, length)
SYSCALL_ARGS_3(int, mprotect, 21, void*, addr, size_t, len, int, prot);
SYSCALL_ARGS_0(int, getpagesize, 22)

#else

#include "interrupts/interrupts.h"

namespace Multitask
{
    extern "C"
    {
        int OnSyscall(const Interrupts::StackFrameRegisters sRegisters);
        extern void IRQ80();
    }
}
#endif

#endif