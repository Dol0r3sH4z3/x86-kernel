#ifndef PMM_H
#define PMM_H

#include <stdint.h>
#include <stddef.h>

void *memset(void *dest, int ch, size_t count);

void pmm_init(void);
void pmm_free_region(uint32_t start_addr, uint32_t length);

void *pmm_alloc_page(void);
void pmm_free_page(void *addr);

#endif