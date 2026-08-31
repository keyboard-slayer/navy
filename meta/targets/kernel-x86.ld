OUTPUT_FORMAT(elf32-i386)
ENTRY(_start)

SECTIONS
{
	. = 1M;

    text_start_addr = .;
	.text ALIGN(4K) : ALIGN(4K)
	{
		*(.multiboot)
		*(.text)
	}
    text_end_addr = .;

    rodata_start_addr = .;
	.rodata ALIGN(4K) : ALIGN(4K)
	{
		*(.rodata)
	}
    rodata_end_addr = .;

    data_start_addr = .;
	.data ALIGN(4K) : ALIGN(4K)
	{
		*(.data)
	}

	.bss ALIGN(4K) : ALIGN(4K)
	{
		*(COMMON)
		*(.bss)
	}
    data_end_addr = .;
}

