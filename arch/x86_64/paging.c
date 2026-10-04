#include <kernel/pmm.h>
#include <arch/arch.h>

#include <stdbool.h>

#define DIRECT_MAP_BASE 0xFFFF800000000000ULL
#define KERNEL_VMA 0xFFFFFFFF80000000ULL

#define ADDR_MASK 0x000FFFFFFFFFF000ULL
#define PTE_HUGE 0x80

typedef uint64_t pte_t;

static pte_t *next_table(pte_t *table, size_t idx, bool create)
{
    pte_t e = table[idx];

    if (!(e & ARCH_PAGE_PRESENT))
    {
        if (!create)
            return NULL;

        phys_addr_t p = pmm_alloc_page();
        if (p == 0)
            return NULL;

        pte_t *t = arch_phys_to_virt(p);
        for (size_t i = 0; i < 512; i++)
            t[i] = 0;

        table[idx] = p | ARCH_PAGE_PRESENT | ARCH_PAGE_WRITE;
        return t;
    }

    if (e & PTE_HUGE)
        return NULL;

    return arch_phys_to_virt(e & ADDR_MASK);
}

void arch_map_page(uintptr_t virt, phys_addr_t phys, uint32_t flags)
{
    uint64_t cr3;
    __asm__ volatile("mov %%cr3, %0" : "=r"(cr3));

    pte_t *pml4 = arch_phys_to_virt(cr3 & ADDR_MASK);
    pte_t *pdpt = next_table(pml4, (virt >> 39) & 0x1FF, true);
    if (!pdpt)
        return;
    pte_t *pd = next_table(pdpt, (virt >> 30) & 0x1FF, true);
    if (!pd)
        return;
    pte_t *pt = next_table(pd, (virt >> 21) & 0x1FF, true);
    if (!pt)
        return;

    pt[(virt >> 12) & 0x1FF] = (phys & ADDR_MASK) | flags | ARCH_PAGE_PRESENT;
    __asm__ volatile("invlpg (%0)" ::"r"(virt) : "memory");
};

void arch_unmap_page(uintptr_t virt)
{
    uint64_t cr3;
    __asm__ volatile("mov %%cr3, %0" : "=r"(cr3));

    pte_t *pml4 = arch_phys_to_virt(cr3 & ADDR_MASK);
    pte_t *pdpt = next_table(pml4, (virt >> 39) & 0x1FF, false);
    if (!pdpt)
        return;
    pte_t *pd = next_table(pdpt, (virt >> 30) & 0x1FF, false);
    if (!pd)
        return;
    pte_t *pt = next_table(pd, (virt >> 21) & 0x1FF, false);
    if (!pt)
        return;

    pte_t *entry = &pt[(virt >> 12) & 0x1FF];
    if (!(*entry & ARCH_PAGE_PRESENT))
        return;

    phys_addr_t phys = *entry & ADDR_MASK;
    *entry = 0;
    pmm_free_page(phys);
    __asm__ volatile("invlpg (%0)" ::"r"(virt) : "memory");
}

void *arch_phys_to_virt(phys_addr_t p) { return (void *)(uintptr_t)(p + DIRECT_MAP_BASE); }
phys_addr_t arch_virt_to_phys(const void *v)
{
    uint64_t a = (uint64_t)(uintptr_t)v;
    if (a >= KERNEL_VMA)
        return a - KERNEL_VMA;
    return a - DIRECT_MAP_BASE;
}