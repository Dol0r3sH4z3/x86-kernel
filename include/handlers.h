#ifndef HANDLERS_H
#define HANDLERS_H

#include <stdint.h>

static inline uint8_t inb(uint16_t port)
{
    uint8_t ret;
    asm volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

__attribute__((noreturn)) void exception_handler(void);
void keyboard_handler_main(void);

#endif
