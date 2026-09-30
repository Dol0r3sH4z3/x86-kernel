#ifndef SLAB_H
#define SLAB_H

#include <stdint.h>

typedef struct Slab
{
    struct Slab *next;
    void *free_list;
    uint32_t cache_idx;
    uint32_t used;
} __attribute__((packed)) Slab;

void slab_init(void);

Slab *slab_new(int idx);

void *kmalloc(uint32_t size);
void kfree(void *p);

#endif