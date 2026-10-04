#include <kernel/pmm.h>
#include <kernel/console.h>
#include <arch/arch.h>
#include <types.h>
#include "arch_config.h"

#define MAX_ORDER 11

list_head_t free_areas[MAX_ORDER];

void pmm_init(void)
{
    for (int i = 0; i < MAX_ORDER; i++)
    {
        free_areas[i].next = &free_areas[i];
        free_areas[i].prev = &free_areas[i];
    }
}

void pmm_free_region(phys_addr_t start_addr, size_t length)
{
    phys_addr_t end = (start_addr + length) & ARCH_PAGE_MASK;
    if (end > ARCH_DIRECT_MAP_LIMIT)
        end = ARCH_DIRECT_MAP_LIMIT;
    start_addr = (start_addr + ARCH_PAGE_SIZE - 1) & ARCH_PAGE_MASK;

    while (start_addr < end)
    {
        int order = 0;
        while (order < MAX_ORDER - 1 &&
               (start_addr & ((ARCH_PAGE_SIZE << (order + 1)) - 1)) == 0 &&
               start_addr + (ARCH_PAGE_SIZE << (order + 1)) <= end)
        {
            order++;
        }

        list_head_t *node = (list_head_t *)arch_phys_to_virt(start_addr);
        node->next = free_areas[order].next;
        node->prev = &free_areas[order];
        free_areas[order].next->prev = node;
        free_areas[order].next = node;

        start_addr += ARCH_PAGE_SIZE << order;
    }
}

phys_addr_t pmm_alloc_page(void)
{
    int current_order = 0;
    while (current_order < MAX_ORDER && free_areas[current_order].next == &free_areas[current_order])
    {
        current_order++;
    }

    if (current_order == MAX_ORDER)
    {
        t_print("Returning because exceeded max order.\n");
        return (phys_addr_t)0;
    }

    list_head_t *block = free_areas[current_order].next;

    block->next->prev = block->prev;
    block->prev->next = block->next;

    while (current_order > 0)
    {
        current_order--;

        size_t buddy_chunk_size = (1U << current_order) * ARCH_PAGE_SIZE;

        uintptr_t buddy_addr = (uintptr_t)block + buddy_chunk_size;
        list_head_t *buddy = (list_head_t *)buddy_addr;

        buddy->next = free_areas[current_order].next;
        buddy->prev = &free_areas[current_order];
        free_areas[current_order].next->prev = buddy;
        free_areas[current_order].next = buddy;
    }

    return arch_virt_to_phys(block);
}

void pmm_free_page(phys_addr_t addr)
{
    uintptr_t block_addr = (uintptr_t)arch_phys_to_virt((uintptr_t)addr);
    int order = 0;
    while (order < MAX_ORDER - 1)
    {
        size_t chunk_size = (1U << order) * ARCH_PAGE_SIZE;

        uintptr_t buddy_addr = block_addr ^ chunk_size;

        list_head_t *buddy = NULL;
        list_head_t *curr = free_areas[order].next;

        while (curr != &free_areas[order])
        {
            if ((uintptr_t)curr == buddy_addr)
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