#include <kernel/console.h>
#include <kernel/pmm.h>
#include <kernel/slab.h>
#include <arch/arch.h>

static inline void test_memory(void)
{
    *(volatile int *)0xFFFF9FFF00000000 = 1;
}

void kernel_main(void)
{
    pmm_init();
    arch_memory_init();
    t_print("PMM Initialized.\n");

    slab_init();
    t_print("Slab initialized.\n");

    arch_enable_interrupts();

    t_print("> ");
    console_enable_input();

    test_memory();

    while (1)
        arch_idle();
}