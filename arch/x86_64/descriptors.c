#include "descriptors.h"

extern void load_gdt(dt_ptr_t *ptr);
extern void *isr_stub_table[];
extern void irq_stub_33(void);

static uint64_t gdt[3] __attribute__((aligned(16))) = {
    0,
    0x00AF9A000000FFFFULL,
    0x00CF92000000FFFFULL,
};
static dt_ptr_t gdt_ptr;

static idt_entry_t idt[256] __attribute__((aligned(16)));
static dt_ptr_t idt_ptr;

function_status_t init_gdt(void)
{
    gdt_ptr.limit = sizeof(gdt) - 1;
    gdt_ptr.base = (uint64_t)(uintptr_t)gdt;
    load_gdt(&gdt_ptr);
    return FUNCTION_STATUS_SUCCESS;
}

void idt_set_descriptor(uint8_t vector, void *isr, uint8_t flags)
{
    uint64_t a = (uint64_t)(uintptr_t)isr;
    idt[vector].offset_low = a & 0xFFFF;
    idt[vector].selector = 0x08;
    idt[vector].ist = 0;
    idt[vector].attributes = flags;
    idt[vector].offset_mid = (a >> 16) & 0xFFFF;
    idt[vector].offset_high = a >> 32;
    idt[vector].reserved = 0;
}

void init_idt(void)
{
    for (uint8_t v = 0; v < 32; v++)
        idt_set_descriptor(v, isr_stub_table[v], 0x8E);
    idt_set_descriptor(33, (void *)irq_stub_33, 0x8E);

    idt_ptr.limit = sizeof(idt) - 1;
    idt_ptr.base = (uint64_t)(uintptr_t)idt;
    __asm__ volatile("lidt %0" ::"m"(idt_ptr));
}