#ifndef STDLIB_H
#define STDLIB_H

#include <stdint.h>
#include <stddef.h>
#include <stdout/uart.h>

size_t strlen(const char* string);
size_t pow(const size_t number, const size_t power);
void memset(void* b, int c, int len);
void memcpy(void *dest, void *src, size_t n);

#define assert(expr) \
    if (!(expr)) \
        error(__FILE__, __LINE__, #expr)

inline void error(const char* file, unsigned int line, const char* expression)
{
    UART::WriteString("Assertion failed in ");
    UART::WriteString(file);
    UART::WriteString(" - line ");
    UART::WriteNumber(line);
    UART::WriteString(": ");
    UART::WriteString(expression);
    UART::WriteString("\n");
}

#endif