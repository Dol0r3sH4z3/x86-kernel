#ifndef PMM_H
#define PMM_H

#include <stdint.h>
#include <arch/arch.h>

void pmm_init(void);
void pmm_free_region(phys_addr_t start_addr, size_t length);

phys_addr_t pmm_alloc_page(void);
void pmm_free_page(void *addr);

#endif