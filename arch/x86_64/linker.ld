ENTRY(_start)
KERNEL_VMA = 0xFFFFFFFF80000000;

SECTIONS
{
    . = 1M;

    .boot : ALIGN(4K)
    {
        KEEP(*(.multiboot.data))
        *(.multiboot.text)
        . = ALIGN(4K);
        *(.boot.bss)
    }

    . += KERNEL_VMA;

    .text : AT(ADDR(.text) - KERNEL_VMA) ALIGN(4K) { *(.text .text.*) }
    .rodata : AT(ADDR(.rodata) - KERNEL_VMA) ALIGN(4K) { *(.rodata .rodata.*) }
    .data : AT(ADDR(.data) - KERNEL_VMA) ALIGN(4K) { *(.data .data.*) }
    .bss : AT(ADDR(.bss) - KERNEL_VMA) ALIGN(4K)
    {
        *(COMMON)
        *(.bss .bss.*)
    }

    _kernel_end = .;
    /DISCARD/ : { *(.eh_frame) *(.comment) }
}