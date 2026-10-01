#include "vga.h"
#include "descriptors.h"
#include "pic.h"
#include "console.h"
#include "pmm.h"
#include "vmm.h"
#include "slab.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define MULTIBOOT_MEMORY_AVAILABLE 1

extern uint32_t _kernel_start;
extern uint32_t _kernel_end;

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
    terminal_print("Kernel started.\n");

    terminal_print("Initializing Global Descriptor Table...\n");
    function_status_t gdt_status = init_gdt();
    if (gdt_status == FUNCTION_STATUS_ERROR)
    {
        terminal_error("Error initializing GDT.\n");
        idle_kernel_loop();
    }
    terminal_print("GDT Initialized successfully.\n\n");
    terminal_print("Remapping PIC...\n");
    PIC_remap(0x20, 0x28);

    terminal_print("Initializing IDT...\n");
    init_idt();

    terminal_print("IDT Initialized successfully\n");
}

static inline void initialize_memory(multiboot_info_t *mbd_phys)
{
    multiboot_info_t *mbd = p2v((uint32_t)mbd_phys);

    if (!(mbd->flags & (1 << 6)))
    {
        terminal_error("[ERROR] GRUB didn't provide memory map.\n");
        __asm__ volatile("cli; hlt");
    }

    multiboot_memory_map_t *mmap = (multiboot_memory_map_t *)p2v(mbd->mmap_addr);
    uint32_t mmap_end = (uint32_t)mmap + mbd->mmap_length;

    uint32_t k_end = v2p(&_kernel_end);

    pmm_init();
    terminal_print("PMM Initialized.\n");
    while ((uint32_t)mmap < mmap_end)
    {

        if (mmap->type == MULTIBOOT_MEMORY_AVAILABLE)
        {

            uint32_t start_address = (uint32_t)mmap->addr;
            uint32_t length_bytes = (uint32_t)mmap->len;

            if (start_address < k_end && (start_address + length_bytes) > k_end)
            {
                uint32_t overlap = k_end - start_address;
                start_address = k_end;
                length_bytes -= overlap;
            }

            if (length_bytes > 0 && start_address >= 0x100000)
            {
                pmm_free_region(start_address, length_bytes);
            }
        }

        mmap = (multiboot_memory_map_t *)((uint32_t)mmap + mmap->size + sizeof(mmap->size));
    }

    terminal_print("VMM Initialized.\n");
    slab_init();
    terminal_print("Slab initialized.\n");
}

void kernel_main(multiboot_info_t *mbd)
{
    vmm_init();
    initialize_services();
    initialize_memory(mbd);
    __asm__ volatile("sti");

    terminal_print("> ");
    console_enable_input();

    idle_kernel_loop();
}