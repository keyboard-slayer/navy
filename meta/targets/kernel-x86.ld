OUTPUT_FORMAT(elf32-i386)
ENTRY(_start)

SECTIONS
{
	. = 1M;

	.text ALIGN(4K) : ALIGN(4K)
	{
		*(.multiboot)
		*(.text)
	}

	.rodata ALIGN(4K) : ALIGN(4K)
	{
		*(.rodata)
	}

	.data ALIGN(4K) : ALIGN(4K)
	{
		*(.data)
	}

	.bss ALIGN(4K) : ALIGN(4K)
	{
		*(COMMON)
		*(.bss)
	}
}

