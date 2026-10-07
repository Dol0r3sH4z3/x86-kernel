# x86-kernel

A hobby x86 kernel written in C and assembly, with a clean split between
architecture-specific code (`arch/`) and portable kernel code (`kernel/`).
Supports both 32-bit protected mode and 64-bit long mode.

## Features
- Multiboot boot via GRUB
- **x86 (32-bit)**: higher-half kernel (`0xC0000000`), 2-level paging
- **x86_64 (64-bit)**: transition from protected mode to long mode,
  4-level paging, higher-half kernel (`0xFFFFFFFF80000000`),
  direct map of physical memory (`0xFFFF800000000000`)
- GDT, IDT, exception handlers, PIC remapping
- Keyboard driver (IRQ1)
- VGA text-mode console with a tiny built-in shell (`help`, `clear`)
- Physical memory manager: buddy allocator
- Kernel heap: slab-style `kmalloc` / `kfree` (32 B to 2 KB)

## Project layout
    arch/x86/       32-bit code: boot, paging, GDT/IDT, drivers
    arch/x86_64/    64-bit code: same, for long mode
    kernel/         architecture-independent: pmm, slab, console, main
    include/arch/   interface between kernel/ and arch/ (arch.h)
    include/kernel/ kernel headers

`kernel/` never touches hardware directly. Everything architecture-specific
(page tables, port I/O, interrupts) goes through `include/arch/arch.h`.

## Build & run
    make                     # build 32-bit kernel
    make ARCH=x86_64         # build 64-bit kernel
    make iso                 # build bootable ISO
    make run                 # build ISO and run in QEMU
    make ARCH=x86_64 run     # same, 64-bit
    make clean

## Requirements
- `i686-elf-gcc` (32-bit) and/or `x86_64-elf-gcc` (64-bit) cross-compilers
- `nasm`, `objcopy` (from the cross binutils)
- `grub-mkrescue` (with `xorriso` and `mtools`)
- `qemu-system-i386` / `qemu-system-x86_64`

## Limitations / roadmap
- Physical memory above 1 GiB is not used on x86_64 (direct map covers 1 GiB)
- No scheduler / multitasking yet
- No userspace, no filesystem
