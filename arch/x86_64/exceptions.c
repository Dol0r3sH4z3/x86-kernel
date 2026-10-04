#include <kernel/console.h>
#include <arch/arch.h>
#include "exceptions.h"

void exception_handler(registers_t *regs)
{
    t_error("Kernel panic! Vector: ");
    t_hex(regs->int_no);
    t_print(" RIP: ");
    t_hex(regs->rip);

    if (regs->int_no == 14)
    {
        uint64_t cr2;
        __asm__ volatile("mov %%cr2, %0" : "=r"(cr2));
        t_error("\nPage fault at: ");
        t_hex(cr2);
    }
    t_print("\n");
    arch_halt();
}