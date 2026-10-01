#include "vmm.h"
#include "pmm.h"
#include "vga.h"

#define PAGE_PRESENT 0x1
#define PAGE_WRITEABLE 0x2

#define DIRECT_MAP_MB 64
#define NUM_PTS (DIRECT_MAP_MB / 4)

static uint32_t *const current_page_directory = (uint32_t *)0xFFFFF000;

static uint32_t kernel_dir[1024] __attribute__((aligned(4096)));
static uint32_t kernel_pts[NUM_PTS][1024] __attribute__((aligned(4096)));

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
        memset(new_pt_virt, 0, 4096);
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

        kernel_dir[768 + t] = v2p(kernel_pts[t]) | PAGE_PRESENT | PAGE_WRITEABLE;
    }

    kernel_dir[1023] = v2p(kernel_dir) | PAGE_PRESENT | PAGE_WRITEABLE;

    __asm__ volatile("mov %0, %%cr3" ::"r"(v2p(kernel_dir)) : "memory");
}