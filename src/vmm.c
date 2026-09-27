#include "vmm.h"
#include "pmm.h"

#define PAGE_PRESENT 0x1
#define PAGE_WRITEABLE 0x2

static uint32_t *const current_page_directory = (uint32_t *)0xFFFFF000;

extern uint32_t kernel_end;

extern void enable_paging(uint32_t *directory);

void vmm_map_page(uint32_t virtual_address, uint32_t physical_addres, uint32_t flags)
{
    uint32_t pd_idx = virtual_address >> 22;
    uint32_t pt_idx = (virtual_address >> 12) & 0x3FF;

    if ((current_page_directory[pd_idx] & PAGE_PRESENT) == 0)
    {
        uint32_t new_pt_phys = (uint32_t)pmm_alloc_page();
        current_page_directory[pd_idx] = new_pt_phys | flags | PAGE_PRESENT;

        uint32_t *new_pt_virt = (uint32_t *)(0xFFC00000 + (pd_idx * 4096));

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
    uint32_t *boot_dir_phys = (uint32_t *)pmm_alloc_page();
    for (uint32_t i = 0; i < 1024; i++)
    {
        boot_dir_phys[i] = 0x00000002;
    }

    boot_dir_phys[1023] = ((uint32_t)boot_dir_phys) | PAGE_PRESENT | PAGE_WRITEABLE;

    uint32_t *kernel_pt_phys = (uint32_t *)pmm_alloc_page();
    for (uint32_t i = 0; i < 1024; i++)
    {
        kernel_pt_phys[i] = 0x00000002;
    }

    uint32_t k_end = (uint32_t)&kernel_end;

    uint32_t start_page = 0;
    uint32_t end_page = (k_end + 4095) / 4096;

    for (uint32_t i = start_page; i < end_page; i++)
    {
        kernel_pt_phys[i] = (i * 4096) | PAGE_PRESENT | PAGE_WRITEABLE;
    }

    boot_dir_phys[0] = ((uint32_t)kernel_pt_phys) | PAGE_PRESENT | PAGE_WRITEABLE;

    enable_paging(boot_dir_phys);
}