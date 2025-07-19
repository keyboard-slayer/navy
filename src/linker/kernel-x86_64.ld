OUTPUT_FORMAT(elf64-x86-64)
ENTRY(handoverEntry)

SECTIONS
{
    . = 0xffffffff80000000;

	.text : ALIGN(4K)
	{
		*(.text)
	}

    .rodata : ALIGN(4K)
    {
        *(.rodata .rodata.*)
    } :rodata

	.data : ALIGN(4K)
	{
		*(.data)
	}

	.bss : ALIGN(4K)
	{
		*(COMMON)
		*(.bss)
	}

    /DISCARD/ : {
        *(.eh_frame)
        *(.note .note.*)
    }
}
