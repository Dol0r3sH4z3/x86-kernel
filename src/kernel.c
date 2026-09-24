#include "vga.h"
#include "descriptors.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

static inline void idle_kernel_loop(void)
{
    while (1)
    {
        __asm__ volatile("hlt");
    }
}

void kernel_main(void)
{
    terminal_initialize();
    terminal_writestring("Kernel started.\n");

    terminal_writestring("Initializing Global Descriptor Table...\n");
    function_status_t gdt_status = initialize_gdt();
    if (gdt_status == FUNCTION_STATUS_ERROR)
    {
        terminal_error("Error initializing GDT.\n");
        idle_kernel_loop();
    }
    terminal_writestring("GDT Initialized successfully.\n");
    terminal_writestring("Initializing IDT...\n");

    initialize_idt();

    terminal_writestring("IDT Initialized successfully\n");

    idle_kernel_loop();
}