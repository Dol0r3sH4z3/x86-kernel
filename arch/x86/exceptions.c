#include <kernel/console.h>
#include <arch/arch.h>
#include "exceptions.h"

void exception_handler(registers_t *regs)
{
    t_error("Kernel panic! Vector: ");
    t_hex(regs->int_no);
    t_print(" EIP: ");
    t_hex(regs->eip);

    if (regs->int_no == 14)
    {
        uint32_t cr2;
        __asm__ volatile("mov %%cr2, %0" : "=r"(cr2));
        t_error("\nPage fault at: ");
        t_hex(cr2);
    }
    else if (regs->int_no == 13)
    {
        t_error("General Protection fault!");
    }

    arch_halt();
}
