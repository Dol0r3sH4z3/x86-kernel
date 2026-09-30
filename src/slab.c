#include "slab.h"
#include "pmm.h"
#include "vmm.h"
#include "vga.h"

#define SLAB_LEVELS 7
#define SLAB_HEAP_BASE 0x40000000u

static const uint32_t class_sizes[SLAB_LEVELS] = {32, 64, 128, 256, 512, 1024, 2048};
static Slab *partial[SLAB_LEVELS];
static uint32_t slab_next_virt = SLAB_HEAP_BASE;

void slab_init()
{
    for (int i = 0; i < SLAB_LEVELS; i++)
    {
        partial[i] = NULL;
    }
}

Slab *slab_new(int idx)
{
    if (idx < 0 || idx >= SLAB_LEVELS)
    {
        terminal_error("Unknown chunk size!\n");
        return NULL;
    }

    void *phys_page = pmm_alloc_page();
    if (phys_page == NULL)
    {
        terminal_error("Cannot allocate page.\n");
        return NULL;
    }
    uint32_t virt_address = slab_next_virt;
    slab_next_virt += 4096;
    vmm_map_page(virt_address, phys_page, 0x2);

    uint8_t *page = (uint8_t *)virt_address;

    uint32_t size = class_sizes[idx];
    uint32_t chunks_amount = (4096 - sizeof(Slab)) / size;

    Slab *slab = (Slab *)page;
    slab->cache_idx = idx;
    slab->used = 0;
    slab->next = NULL;

    uint8_t *cursor = page + sizeof(Slab);
    for (uint32_t i = 0; i < chunks_amount - 1; i++)
    {
        *(void **)cursor = cursor + size;
        cursor += size;
    }
    *(void **)cursor = NULL;

    slab->free_list = page + sizeof(Slab);

    return slab;
};

void *kmalloc(uint32_t size)
{
    if (size == 0)
        return NULL;

    uint32_t index = 0;
    while (index < SLAB_LEVELS && class_sizes[index] < size)
        index++;

    if (index >= SLAB_LEVELS)
        return NULL;

    if (partial[index] == NULL)
        partial[index] = slab_new(index);

    Slab *slab = partial[index];
    while (slab->free_list == NULL && slab->next != NULL)
        slab = slab->next;

    if (slab->free_list == NULL)
    {
        Slab *new_slab = slab_new(index);
        slab->next = new_slab;
        slab = new_slab;
    }

    void *free_chunk = slab->free_list;
    slab->free_list = *(void **)free_chunk;
    slab->used++;

    return free_chunk;
}
void kfree(void *p)
{
    if (p == NULL)
        return;

    Slab *base_slab = (Slab *)((uint32_t)p & ~0xFFF);

    *(void **)p = base_slab->free_list;
    base_slab->free_list = p;
    base_slab->used--;
};