.section ".text.startup"

.globl _start
_start:
    // fpu
    mov x1, #(0x3 << 20)
    msr cpacr_el1, x1 
    isb

    ldr x30, =STACK_TOP
    mov sp, x30
    bl aarch64_entry 
.hang:
    b .hang