#ifndef ARCH_H
#define ARCH_H

#define ARCH_DIRECT_MAP_LIMIT (64u * 1024 * 1024)
#define ARCH_PAGE_SIZE 4096u
#define ARCH_PAGE_MASK (~(uintptr_t)(ARCH_PAGE_SIZE - 1))
#define ARCH_PAGE_WRITE 0x2
#define ARCH_PAGE_PRESENT 0x1

#include <stdint.h>
#include <stddef.h>

typedef uint64_t phys_addr_t;
typedef enum
{
    COLOR_DEFAULT,
    COLOR_RED,
    COLOR_YELLOW,
    COLOR_GREEN
} arch_color_t;

void *arch_phys_to_virt(phys_addr_t p);
phys_addr_t arch_virt_to_phys(const void *v);

void arch_map_page(uintptr_t virt, phys_addr_t phys, uint32_t flags);
void arch_unmap_page(uintptr_t virt);

void arch_halt(void);
void arch_idle(void);
void arch_enable_interrupts(void);

void arch_memory_init(void);

void arch_console_init(void);
void arch_colsole_putc(char c);
void arch_console_set_color(arch_color_t c);
void arch_console_mark_input_start(void);

#endif