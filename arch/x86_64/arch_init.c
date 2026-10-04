#include <arch/arch.h>
#include <kernel/console.h>

void arch_halt() { __asm__ volatile("cli; hlt"); }
void arch_idle() { __asm__ volatile("hlt"); }
void arch_enable_interrupts() { __asm__ volatile("sti"); }

void kernel_main(void);

void arch_main(uint32_t mbd_phys, uint32_t magic)
{
    (void)mbd_phys;
    (void)magic;
    arch_console_init();
    kernel_main();
}

void arch_memory_init() {}