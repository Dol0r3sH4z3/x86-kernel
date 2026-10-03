#include <arch/arch.h>
#include <kernel/console.h>
#include <kernel/pmm.h>
#include "multiboot.h"
#include "descriptors.h"
#include "paging.h"
#include "pic.h"

void arch_halt() { __asm__ volatile("cli; hlt"); }
void arch_idle() { __asm__ volatile("hlt"); }
void arch_enable_interrupts() { __asm__ volatile("sti"); }

extern char _kernel_end[];
void kernel_main(void);

static multiboot_info_t *boot_info;

void arch_main(multiboot_info_t *mbd_phys)
{
    vmm_init();
    boot_info = arch_phys_to_virt((uint32_t)mbd_phys);

    arch_console_init();
    t_print("Kernel started.\n");

    if (init_gdt() == FUNCTION_STATUS_ERROR)
    {
        t_error("Error initializing GDT.\n");
        arch_halt();
    }
    PIC_remap(0x20, 0x28);

    t_print("Initializing IDT...\n");
    init_idt();
    t_print("IDT Initialized successfully\n");

    kernel_main();
}

void arch_memory_init()
{
    if (!(boot_info->flags & (1 << 6)))
    {
        t_error("[ERROR] GRUB didn't provide memory map.\n");
        arch_halt();
    }

    multiboot_memory_map_t *mmap = (multiboot_memory_map_t *)arch_phys_to_virt(boot_info->mmap_addr);
    uint32_t mmap_end = (uint32_t)mmap + boot_info->mmap_length;
    uint32_t k_end = arch_virt_to_phys(_kernel_end);

    while ((uint32_t)mmap < mmap_end)
    {

        if (mmap->type == 1)
        {

            uint32_t start = (uint32_t)mmap->addr;
            uint32_t len = (uint32_t)mmap->len;

            if (start < k_end && start + len > k_end)
            {
                len -= k_end - start;
                start = k_end;
            }

            if (len > 0 && start >= 0x100000)
            {
                pmm_free_region(start, len);
            }
        }

        mmap = (multiboot_memory_map_t *)((uint32_t)mmap + mmap->size + sizeof(mmap->size));
    }
}