#include "descriptors.h"
#include "vga.h"

#include <stdbool.h>

#define GDT_SIZE 3
#define IDT_MAX_DESCRIPTORS 33

uint8_t gdt_entries[GDT_SIZE * 8];
__attribute__((aligned(0x10))) static idt_entry_t idt[256];

extern void load_gdt(uint32_t gdt_ptr_addr);
extern void irq_stub_33(void);
extern void *isr_stub_table[];

static bool vectors[IDT_MAX_DESCRIPTORS];

function_status_t encodeGdtEntry(uint8_t *target, struct GDT source)
{
    // Check the limit to make sure that it can be encoded
    if (source.limit > 0xFFFFF)
    {
        terminal_writestring("GDT cannot encode limits larger than 0xFFFFF\n");
        return FUNCTION_STATUS_ERROR;
    }

    // Encode the limit
    target[0] = source.limit & 0xFF;
    target[1] = (source.limit >> 8) & 0xFF;
    target[6] = (source.limit >> 16) & 0x0F;

    // Encode the base
    target[2] = source.base & 0xFF;
    target[3] = (source.base >> 8) & 0xFF;
    target[4] = (source.base >> 16) & 0xFF;
    target[7] = (source.base >> 24) & 0xFF;

    // Encode the access byte
    target[5] = source.access_byte;

    // Encode the flags
    target[6] |= (source.flags << 4);

    return FUNCTION_STATUS_SUCCESS;
}

function_status_t initialize_gdt()
{
    struct GDT source;

    source.access_byte = 0;
    source.limit = 0;
    source.flags = 0;
    source.base = 0;
    function_status_t null_descriptor_status = encodeGdtEntry(&gdt_entries[0], source);
    if (null_descriptor_status == FUNCTION_STATUS_ERROR)
    {
        terminal_error("Error initializing null descriptor.\n");
        return FUNCTION_STATUS_ERROR;
    }

    terminal_writestring("Initialized first descriptor.\n");

    source.base = 0;
    source.limit = 0xFFFFF;
    source.access_byte = 0x9A;
    source.flags = 0xC;
    function_status_t kernel_code_descriptor_status = encodeGdtEntry(&gdt_entries[8], source);
    if (kernel_code_descriptor_status == FUNCTION_STATUS_ERROR)
    {
        terminal_error("Error initializing kernel code descriptor.\n");
        return FUNCTION_STATUS_ERROR;
    }

    terminal_writestring("Initialized second descriptor.\n");

    source.base = 0;
    source.limit = 0xFFFFF;
    source.access_byte = 0x92;
    source.flags = 0xC;
    function_status_t kernel_data_descriptor_status = encodeGdtEntry(&gdt_entries[16], source);
    if (kernel_data_descriptor_status == FUNCTION_STATUS_ERROR)
    {
        terminal_error("Error initializing kernel data descriptor.\n");
        return FUNCTION_STATUS_ERROR;
    }

    terminal_writestring("Initialized third descriptor.\n");

    static struct gdt_pointer gp;
    gp.limit = (GDT_SIZE * 8) - 1;
    gp.base = (uint32_t)&gdt_entries;

    load_gdt((uint32_t)&gp);

    return FUNCTION_STATUS_SUCCESS;
}

void idt_set_descriptor(uint8_t vector, void *isr, uint8_t flags)
{
    idt_entry_t *descriptor = &idt[vector];

    descriptor->isr_low = (uint32_t)isr & 0xFFFF;
    descriptor->kernel_cs = 0x08;
    descriptor->attributes = flags;
    descriptor->isr_high = (uint32_t)isr >> 16;
    descriptor->reserved = 0;
};

void initialize_idt()
{
    static idtr_t idtr;

    idtr.base = (uintptr_t)&idt[0];
    idtr.limit = (uint16_t)sizeof(idt) - 1;

    for (uint8_t vector = 0; vector < 32; vector++)
    {
        idt_set_descriptor(vector, isr_stub_table[vector], 0x8E);
        vectors[vector] = true;
    }
    idt_set_descriptor(33, (void *)irq_stub_33, 0x8E);

    __asm__ volatile("lidt %0" : : "m"(idtr));
}
