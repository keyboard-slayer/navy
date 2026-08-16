#include <allocators/bump.h>
#include <base64.h>
#include <fmt.h>
#include <logging.h>

#include "e9.h"
#include "int_handler.h"
#include "regs.h"

static char* exception_messages[32] = {
    "Division By Zero",
    "Debug",
    "Non Maskable Interrupt",
    "Breakpoint",
    "Detected Overflow",
    "Out Of Bounds",
    "Invalid Opcode",
    "No Coprocessor",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Bad Tss",
    "Segment Not Present",
    "StackFault",
    "General Protection Fault",
    "Page Fault",
    "Unknown Interrupt",
    "Coprocessor Fault",
    "Alignment Check",
    "Machine Check",
    "SIMD Floating-Point Exception",
    "Virtualization Exception",
    "Control Protection Exception",
    "Reserved",
    "Hypervisor Injection Exception",
    "VMM Communication Exception",
    "Security Exception",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
};

static char addrBuffer[256];

static void kpanic(Regs regs[const static 1]) {
    uint64_t cr0;
    uint64_t cr2;
    uint64_t cr3;
    uint64_t cr4;

    REGS_READ(cr0);
    REGS_READ(cr2);
    REGS_READ(cr3);
    REGS_READ(cr4);

    if (e9_available()) {
        BumpAllocator alloc = bump_allocator_create(addrBuffer, 256);
        Result res_buff = alloc.base.alloc(&alloc.base, 17);
        if (res_buff.type != EOK) {
            goto ignore;
        }

        utoa(regs->rip, unwrap$(res_buff, char*), 16);
        Result b64 = b64encode(unwrap$(res_buff, char*), (Allocator*)&alloc);
        if (b64.type != EOK) {
            goto ignore;
        }

        print$("\033]52;c;%s\033\\", unwrap$(b64, char*));
    }

ignore:
    print$("\n\x1B[0;31m!!!\x1B[0;33m ---------------------------------------------------------------------------------------------------\n\n\x1B[0m");
    print$("    KERNEL PANIC\n");
    print$("    %s was raised\n\n", exception_messages[regs->intno]);
    print$("    Registers:\n");
    print$("    interrupt: %x, err: %x \n", regs->intno, regs->err);
    print$("    RAX %p RBX %p RCX %p RDX %p\n", regs->rax, regs->rbx, regs->rcx, regs->rdx);
    print$("    RSI %p RDI %p RBP %p RSP %p\n", regs->rsi, regs->rdi, regs->rbp, regs->rsp);
    print$("    R8  %p R9  %p R10 %p R11 %p\n", regs->r8, regs->r9, regs->r10, regs->r11);
    print$("    R12 %p R13 %p R14 %p R15 %p\n", regs->r12, regs->r13, regs->r14, regs->r15);
    print$("    CR0 %p CR2 %p CR3 %p CR4 %p\n", cr0, cr2, cr3, cr4);
    print$("    CS  %p SS  %p FLG %p\n", regs->cs, regs->ss, regs->rflags);
    print$("    RIP \033[7m%p\033[0m\n\n", regs->rip);
    print$("    Backrace:\n");
    print$("\n\x1B[0;33m ---------------------------------------------------------------------------------------------------\x1B[0;31m !!!\x1B[0m\n");
}

void interrupt_handler(uint64_t rsp) {
    Regs* regs = (Regs*)rsp;

    if (regs->intno < IRQ0) {
        kpanic(regs);

        for (;;) {
            asm volatile(
                "cli;"
                "hlt;"
            );
        }
    }

    asm volatile("hlt");
}
