#include <arch/arch.h>
#include <kernel/console.h>
#include <kernel/pmm.h>
#include "arch_config.h"
#include "multiboot.h"
#include "descriptors.h"
#include "pic.h"

#define MULTIBOOT_BOOTLOADER_MAGIC 0x2BADB002
#define MULTIBOOT_MEMORY_AVAILABLE 1

void arch_halt(void)
{
    for (;;)
        __asm__ volatile("cli; hlt");
}
void arch_idle(void) { __asm__ volatile("hlt"); }
void arch_enable_interrupts(void) { __asm__ volatile("sti"); }

extern char _kernel_end[];
void kernel_main(void);

static multiboot_info_t *boot_info;

void arch_main(uint32_t mbd_phys, uint32_t magic)
{
    arch_console_init();
    t_print("Kernel started.\n");

    if (magic != MULTIBOOT_BOOTLOADER_MAGIC)
    {
        t_error("Not booted by a Multiboot loader.\n");
        arch_halt();
    }
    boot_info = arch_phys_to_virt(mbd_phys);

    if (init_gdt() == FUNCTION_STATUS_ERROR)
    {
        t_error("Error initializing GDT.\n");
        arch_halt();
    }

    PIC_remap(0x20, 0x28);

    t_print("Initializing IDT...\n");
    init_idt();
    t_print("IDT initialized successfully.\n");

    kernel_main();
}

void arch_memory_init(void)
{
    if (!(boot_info->flags & (1 << 6)))
    {
        t_error("GRUB didn't provide memory map.\n");
        arch_halt();
    }

    uintptr_t mmap_cur = (uintptr_t)arch_phys_to_virt(boot_info->mmap_addr);
    uintptr_t mmap_end = mmap_cur + boot_info->mmap_length;

    uint64_t k_end = arch_virt_to_phys(_kernel_end);

    while (mmap_cur < mmap_end)
    {
        multiboot_memory_map_t *mmap = (multiboot_memory_map_t *)mmap_cur;

        if (mmap->type == MULTIBOOT_MEMORY_AVAILABLE)
        {
            uint64_t start = mmap->addr;
            uint64_t end = mmap->addr + mmap->len;

            if (start < 0x100000)
                start = 0x100000;
            if (start < k_end)
                start = k_end;
            if (end > ARCH_DIRECT_MAP_LIMIT)
                end = ARCH_DIRECT_MAP_LIMIT;

            if (start < end)
                pmm_free_region(start, (size_t)(end - start));
        }

        mmap_cur += mmap->size + sizeof(mmap->size);
    }
}