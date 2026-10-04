#include <kernel/console.h>
#include <arch/arch.h>

void kernel_main(void)
{
    t_print("Hello world!");
    t_print("> ");
    console_enable_input();

    while (1)
        arch_idle();
}