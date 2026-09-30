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

static inline void initialize_memory(multiboot_info_t *mbd)
{
    if (!(mbd->flags & (1 << 6)))
    {
        terminal_error("[ERROR] GRUB didn't provide memory map.\n");
        __asm__ volatile("cli; hlt");
    }

    multiboot_memory_map_t *mmap = (multiboot_memory_map_t *)mbd->mmap_addr;
    uint32_t mmap_end = mbd->mmap_addr + mbd->mmap_length;

    uint32_t k_end = (uint32_t)&kernel_end;

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

    vmm_init();
    terminal_print("VMM Initialized.\n");
    slab_init();
    terminal_print("Slab initialized.\n");
}

static inline void test_slab_memory(void)
{
    void *a = kmalloc(40);
    void *b = kmalloc(40);

    terminal_print("a = ");
    terminal_print_hex((uint32_t)a);
    terminal_print("\n");

    terminal_print("b = ");
    terminal_print_hex((uint32_t)b);
    terminal_print("\n");

    kfree(a);

    void *c = kmalloc(40);
    terminal_print("c = ");
    terminal_print_hex((uint32_t)c);
    terminal_print("\n");

    if (c == a)
        terminal_print("OK: c == a, freelist works!\n");
    else
        terminal_print("FAIL: c != a\n");
}

static inline void test_vmm(void)
{
    void *a = pmm_alloc_page();
    if (a == NULL)
    {
        terminal_print("Cannot initialize a.\n");
        return;
    }

    terminal_print("A addr: ");
    terminal_print_hex((uint32_t)a);
    terminal_print("\n");

    // static uint32_t
}

void kernel_main(multiboot_info_t *mbd)
{
    initialize_services();
    initialize_memory(mbd);
    __asm__ volatile("sti");

    // test_slab_memory();
    void *a = pmm_alloc_page();
    // test_vmm();

    terminal_print("> ");
    console_enable_input();

    idle_kernel_loop();
}