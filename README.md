32-bit x86 hobby kernel written in C and assembly.

## Implemented
- Multiboot boot, entry into `kernel_main`
- GDT, IDT, PIC remapping
- Keyboard interrupts (IRQ1)
- VGA text-mode terminal with a tiny built-in shell (`help`, `clear`)
- Physical memory manager: buddy allocator
- Paging (VMM)
- `kmalloc` / `kfree`: slab-style allocator (32 B to 2 KB)

## Build & run
    make        # build kernel binary
    make iso    # build bootable ISO
    make run    # build ISO and run in QEMU
    make clean  # remove build files

## Requirements
- `i686-elf-gcc` cross-compiler (GCC targeting i686-elf)
- `nasm`
- `grub-mkrescue` (with `xorriso` and `mtools`)
- `qemu-system-i386`