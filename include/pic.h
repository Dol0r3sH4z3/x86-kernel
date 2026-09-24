#ifndef PIC_H
#define PIC_H

#include <stdint.h>

static inline void outb(uint16_t port, uint8_t val)
{
    asm volatile("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline void io_wait(void)
{
    asm volatile("outb %%al, $0x80" : : "a"(0));
}

void PIC_remap(int offset1, int offset2);
void PIC_sendEOI(uint8_t irq);

#endif