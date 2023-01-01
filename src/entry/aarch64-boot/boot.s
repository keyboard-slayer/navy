.section ".text.startup"

.globl _start
_start:
    ldr x30, =STACK_TOP
    mov sp, x30
    bl bootstrap
.hang:
    b .hang