#ifndef SLAB_H
#define SLAB_H

#include <stdint.h>
#include <stddef.h>

#define SLAB_HEADER_SIZE 32u

typedef struct Slab
{
    struct Slab *next;
    void *free_list;
    uint32_t cache_idx;
    uint32_t used;
} __attribute__((packed)) Slab;

_Static_assert(sizeof(Slab) <= SLAB_HEADER_SIZE, "Slab header too big");

void slab_init(void);

Slab *slab_new(int idx);

void *kmalloc(size_t size);
void kfree(void *p);

#endif