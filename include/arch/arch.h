#ifndef ARCH_H
#define ARCH_H

#define ARCH_DIRECT_MAP_LIMIT (64u * 1024 * 1024)

#include <stdint.h>
#include <stddef.h>

typedef uint64_t phys_addr_t;

void *arch_phys_to_virt(phys_addr_t p);
phys_addr_t arch_virt_to_phys(const void *v);

void arch_halt(void);
void arch_idle(void);
void arch_enable_interrupts(void);

void arch_memory_init(void);

#endif