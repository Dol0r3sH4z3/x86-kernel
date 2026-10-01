#include "include/arch/arch.h"

void arch_halt() { __asm__ volatile("cli; hlt"); }
void arch_idle() { __asm__ volatile("hlt"); }
void arch_enable_interrupts() { __asm__ volatile("sti"); }