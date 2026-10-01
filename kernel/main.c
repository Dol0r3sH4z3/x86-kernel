#include <kernel/console.h>
#include <kernel/pmm.h>
#include <kernel/slab.h>
#include <arch/arch.h>
#include "vga.h"

void kernel_main(void)
{
    pmm_init();
    arch_memory_init();
    terminal_print("PMM Initialized.\n");

    slab_init();
    terminal_print("Slab initialized.\n");

    arch_enable_interrupts();

    terminal_print("> ");
    console_enable_input();

    while (1)
        arch_idle();
}