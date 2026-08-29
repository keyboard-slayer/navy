;; Multiboot 2
MULTIBOOT2_HEADER_MAGIC                 equ 0xe85250d6
MULTIBOOT_ARCHITECTURE_I386             equ 0
HEADER_LENGTH                           equ multiboot_end - multiboot_start
CHECKSUM                                equ -(MULTIBOOT2_HEADER_MAGIC + MULTIBOOT_ARCHITECTURE_I386 + HEADER_LENGTH)
MULTIBOOT_HEADER_TAG_ADDRESS            equ 2
MULTIBOOT_HEADER_TAG_OPTIONAL           equ 1
MULTIBOOT_TAG_TYPE_FRAMEBUFFER          equ 5
MULTIBOOT_HEADER_TAG_END                equ 0

extern boot_stivale2
extern boot_multiboot2

section .multiboot
align 8
multiboot_start:
    dd MULTIBOOT2_HEADER_MAGIC
    dd MULTIBOOT_ARCHITECTURE_I386
    dd HEADER_LENGTH
    dd CHECKSUM

align 8
    dw MULTIBOOT_HEADER_TAG_END
    dw 0
    dd 8
multiboot_end:

section .bss
    align 16
    stack_bottom:
        resb 16384 ; 16 KiB
    stack_top:

section .text
    global _start:function (_start.end - _start)
    _start:
	cli
    cld

	mov esp, stack_top
    xor ebp, ebp

    push ebx
    push eax

	call boot_multiboot2

.hang:	hlt
	jmp .hang
.end:
