#ifndef PMM_H
#define PMM_H

#include <stdint.h>
#include <stddef.h>

typedef enum
{
    PMM_REGION_FREE,
    PMM_REGION_LOCK
} pmm_action_t;

void *memset(void *dest, int ch, size_t count);

void pmm_init(void);
void pmm_set_region(uint32_t start_addr, uint32_t length, pmm_action_t action);

void *pmm_alloc_page(void);
void pmm_free_page(void *addr);

#endif