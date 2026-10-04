#include <kernel/console.h>
#include <arch/arch.h>
#include "exceptions.h"

void exception_handler(registers_t *regs)
{
    t_error("Kernel panic: ");
    if (regs->int_no == 14)
    {
        t_error("Page fault!");
        uint32_t faulting_address;
        __asm__ volatile("mov %%cr2, %0" : "=r"(faulting_address));
        t_error(" Address: ");
        t_hex(faulting_address);
        t_print("\n");
        arch_halt();
    }
    else if (regs->int_no == 13)
    {
        t_error("General Protection fault!");
        }
}
