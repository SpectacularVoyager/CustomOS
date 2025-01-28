ENTRY(_start)

SECTIONS
{
	. = 0x00100000;
	_kernel_start = .;
	.multiboot : {
		*(.multiboot)
	}

	. += 0xC0000000;
	.text ALIGN (4K) : AT (ADDR (.text) - 0xC0000000)
	{
		*(.text)
	}

	/* Read-only data. */
	.rodata ALIGN (4K) : AT (ADDR (.rodata) - 0xC0000000)
	{
		*(.rodata)
	}

	/* Read-write data (initialized) */
	.data ALIGN (4K) : AT (ADDR (.data) - 0xC0000000)
	{
		*(.data)
	}

	/* Read-write data (uninitialized) and stack */
	.bss ALIGN (4K) : AT (ADDR (.bss) - 0xC0000000)
	{
		*(COMMON)
		*(.bss)
	}
	_kernel_end = .;

	/* The compiler may produce other sections, by default it will put them in
	   a segment with the same name. Simply add stuff here as needed. */
}
