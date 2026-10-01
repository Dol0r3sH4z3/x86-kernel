#include <kernel/pmm.h>
#include <arch/arch.h>
#include "paging.h"
#include "vga.h"

#define PAGE_PRESENT 0x1
#define PAGE_WRITEABLE 0x2

#define DIRECT_MAP_MB 64
#define NUM_PTS (DIRECT_MAP_MB / 4)

#define KERNEL_VMA 0xC0000000

typedef uint32_t pte_t;

static uint32_t *const current_page_directory = (uint32_t *)0xFFFFF000;

static pte_t kernel_dir[1024] __attribute__((aligned(4096)));
static pte_t kernel_pts[NUM_PTS][1024] __attribute__((aligned(4096)));

void vmm_map_page(uint32_t virtual_address, uint32_t physical_addres, uint32_t flags)
{
    uint32_t pd_idx = virtual_address >> 22;
    uint32_t pt_idx = (virtual_address >> 12) & 0x3FF;

    if ((current_page_directory[pd_idx] & PAGE_PRESENT) == 0)
    {
        uint32_t new_pt_phys = (uint32_t)pmm_alloc_page();
        current_page_directory[pd_idx] = new_pt_phys | flags | PAGE_PRESENT;

        uint32_t *new_pt_virt = (uint32_t *)(0xFFC00000 + (pd_idx * 4096));

        __asm__ volatile("invlpg (%0)" ::"r"(new_pt_virt) : "memory");
        for (size_t i = 0; i < 1024; i++)
        {
            new_pt_virt[i] = 0x0;
        }
    }

    uint32_t *page_table = (uint32_t *)(0xFFC00000 + (pd_idx * 4096));
    page_table[pt_idx] = physical_addres | flags | PAGE_PRESENT;
    __asm__ volatile("invlpg (%0)" ::"r"(virtual_address) : "memory");
};

void vmm_unmap_page(uint32_t virtual_address)
{
    uint32_t pd_idx = virtual_address >> 22;
    uint32_t pt_idx = (virtual_address >> 12) & 0x3FF;

    if ((current_page_directory[pd_idx] & PAGE_PRESENT) == 0)
        return;

    uint32_t *page_table = (uint32_t *)(0xFFC00000 + (pd_idx * 4096));
    uint32_t physical_address = page_table[pt_idx] & ~0xFFF;

    page_table[pt_idx] &= ~0x00000001;
    pmm_free_page((void *)physical_address);

    __asm__ volatile("invlpg (%0)" ::"r"(virtual_address) : "memory");
}

void vmm_init()
{
    for (uint32_t t = 0; t < NUM_PTS; t++)
    {
        for (uint32_t i = 0; i < 1024; i++)
        {
            uint32_t phys = (t * 1024 + i) * 4096;
            kernel_pts[t][i] = phys | PAGE_WRITEABLE | PAGE_PRESENT;
        }

        kernel_dir[768 + t] = arch_virt_to_phys(kernel_pts[t]) | PAGE_PRESENT | PAGE_WRITEABLE;
    }

    kernel_dir[1023] = arch_virt_to_phys(kernel_dir) | PAGE_PRESENT | PAGE_WRITEABLE;

    __asm__ volatile("mov %0, %%cr3" ::"r"((uint32_t)arch_virt_to_phys(kernel_dir)) : "memory");
}

void *arch_phys_to_virt(phys_addr_t p) { return (void *)((uintptr_t)p + KERNEL_VMA); }
phys_addr_t arch_virt_to_phys(const void *v) { return (phys_addr_t)((uintptr_t)v - KERNEL_VMA); }