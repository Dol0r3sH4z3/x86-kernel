#include "interrupts.h"
#include "vga.h"

#define GDT_SIZE 3

uint8_t gdt_entries[GDT_SIZE * 8];
extern void load_gdt(uint32_t gdt_ptr_addr);

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

function_status_t initialize_gdt(void)
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
    source.flags = 0x0C;
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
    source.flags = 0x0C;
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