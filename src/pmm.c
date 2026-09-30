#include "pmm.h"
#include "types.h"

#define MAX_ORDER 11

list_head_t free_areas[MAX_ORDER];

void *memset(void *dest, int ch, size_t count)
{
    uint8_t *ptr = (uint8_t *)dest;
    while (count > 0)
    {
        *ptr = (uint8_t)ch;
        ptr++;
        count--;
    }
    return dest;
}

void pmm_init(void)
{
    for (int i = 0; i < MAX_ORDER; i++)
    {
        free_areas[i].next = &free_areas[i];
        free_areas[i].prev = &free_areas[i];
    }
};
void pmm_free_region(uint32_t start_addr, uint32_t length)
{
    uint32_t end = (start_addr + length) & ~0xFFFu;
    start_addr = (start_addr + 0xFFF) & ~0xFFFu;

    while (start_addr < end)
    {
        uint32_t order = 0;
        while (order < MAX_ORDER - 1 &&
               (start_addr & ((4096u << (order + 1)) - 1)) == 0 &&
               start_addr + (4096u << (order + 1)) <= end)
        {
            order++;
        }

        list_head_t *node = (list_head_t *)start_addr;
        node->next = free_areas[order].next;
        node->prev = &free_areas[order];
        free_areas[order].next->prev = node;
        free_areas[order].next = node;

        start_addr += 4096u << order;
    }
};

void *pmm_alloc_page(void)
{
    int current_order = 0;
    while (current_order < MAX_ORDER && free_areas[current_order].next == &free_areas[current_order])
    {
        current_order++;
    }

    if (current_order == MAX_ORDER)
    {
        return NULL;
    }

    list_head_t *block = free_areas[current_order].next;

    block->next->prev = block->prev;
    block->prev->next = block->next;

    while (current_order > 0)
    {
        current_order--;

        uint32_t buddy_chunk_size = (1U << current_order) * 4096;

        uint32_t buddy_addr = (uint32_t)block + buddy_chunk_size;
        list_head_t *buddy = (list_head_t *)buddy_addr;

        buddy->next = free_areas[current_order].next;
        buddy->prev = &free_areas[current_order];
        free_areas[current_order].next->prev = buddy;
        free_areas[current_order].next = buddy;
    }

    return (void *)block;
};
void pmm_free_page(void *addr)
{
    uint32_t block_addr = (uint32_t)addr;
    uint32_t order = 0;
    while (order < MAX_ORDER - 1)
    {
        uint32_t chunk_size = (1U << order) * 4096;

        uint32_t buddy_addr = block_addr ^ chunk_size;

        list_head_t *buddy = NULL;
        list_head_t *curr = free_areas[order].next;

        while (curr != &free_areas[order])
        {
            if ((uint32_t)curr == buddy_addr)
            {
                buddy = curr;
                break;
            }
            curr = curr->next;
        }

        if (buddy == NULL)
        {
            break;
        }

        buddy->next->prev = buddy->prev;
        buddy->prev->next = buddy->next;

        if (buddy_addr < block_addr)
        {
            block_addr = buddy_addr;
        }

        order++;
    }

    list_head_t *final_node = (list_head_t *)block_addr;

    final_node->next = free_areas[order].next;
    final_node->prev = &free_areas[order];
    free_areas[order].next->prev = final_node;
    free_areas[order].next = final_node;
}