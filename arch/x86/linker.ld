ENTRY(_start)

SECTIONS
{
	. = 0x00100000;
	_kernel_start = .;

	.multiboot.data : {
		*(.multiboot.data)
	}
	
	.multiboot.text : {
		*(.multiboot.text)
	}

	. += 0xC0000000;
	.text BLOCK(4K) : AT (ADDR (.text) - 0xC0000000)
	{
		*(.text)
	}

	/* Read-only data. */
	.rodata BLOCK(4K) : AT (ADDR (.rodata) - 0xC0000000)
	{
		*(.rodata)
	}

	.data BLOCK(4K) : AT (ADDR (.data) - 0xC0000000)
	{
		*(.data)
	}

	.bss BLOCK(4K) : AT (ADDR (.bss) - 0xC0000000)
	{
		*(COMMON)
		*(.bss)
		*(.bootstrap_stack)
	}


	_kernel_end = .;
}