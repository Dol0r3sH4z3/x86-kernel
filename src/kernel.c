#include "vga.h"
#include "descriptors.h"
#include "pic.h"
#include "console.h"
#include "pmm.h"
#include "vmm.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define MULTIBOOT_MEMORY_AVAILABLE 1

extern uint32_t kernel_start;
extern uint32_t kernel_end;

static inline void idle_kernel_loop(void)
{
    while (1)
    {
        __asm__ volatile("hlt");
    }
}

static inline void initialize_services(void)
{
    init_term();
    terminal_writestring("Kernel started.\n");

    terminal_writestring("Initializing Global Descriptor Table...\n");
    function_status_t gdt_status = init_gdt();
    if (gdt_status == FUNCTION_STATUS_ERROR)
    {
        terminal_error("Error initializing GDT.\n");
        idle_kernel_loop();
    }
    terminal_writestring("GDT Initialized successfully.\n\n");
    terminal_writestring("Remapping PIC...\n");
    PIC_remap(0x20, 0x28);

    terminal_writestring("Initializing IDT...\n");
    init_idt();

    terminal_writestring("IDT Initialized successfully\n");
}

static inline void initialize_memory(multiboot_info_t *mbd)
{
    if (!(mbd->flags & (1 << 6)))
    {
        terminal_error("[ERROR] GRUB didn't provide memory map.\n");
        __asm__ volatile("cli; hlt");
    }

    multiboot_memory_map_t *mmap = (multiboot_memory_map_t *)mbd->mmap_addr;
    uint32_t mmap_end = mbd->mmap_addr + mbd->mmap_length;

    pmm_init();
    while ((uint32_t)mmap < mmap_end)
    {

        if (mmap->type == MULTIBOOT_MEMORY_AVAILABLE)
        {

            uint32_t start_address = (uint32_t)mmap->addr;
            uint32_t length_bytes = (uint32_t)mmap->len;

            pmm_set_region(start_address, length_bytes, PMM_REGION_FREE);
        }

        mmap = (multiboot_memory_map_t *)((uint32_t)mmap + mmap->size + sizeof(mmap->size));
    }

    uint32_t k_start = (uint32_t)&kernel_start;
    uint32_t k_end = (uint32_t)&kernel_end;

    pmm_set_region(k_start, k_end - k_start, PMM_REGION_LOCK);
    pmm_set_region(0x0, 0x100000, PMM_REGION_LOCK);

    vmm_init();
}

void kernel_main(multiboot_info_t *mbd)
{
    initialize_memory(mbd);
    initialize_services();
    __asm__ volatile("sti");

    terminal_writestring("> ");
    console_enable_input();

    idle_kernel_loop();
}