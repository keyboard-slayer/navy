#include <allocators/freelist.h>
#include <arch/interface.h>
#include <base64.h>
#include <fmt.h>
#include <logging.h>

#include "../x86-common/e9.h"
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

static Elf_Ehdr kernel_elf = {0};
static bool kernel_elf_acquired = false;

static Elf_Sym get_symbol(uintptr_t addr) {
    if (!kernel_elf_acquired) {
        kernel_elf_acquired = true;
        Option kbin = get_kernel_elf();
        if (is_none(kbin)) {
            warn$("Couldn't get kernel symbols");
        }

        kernel_elf = elf_parse((uint8_t*)kbin.value);
    }

    if (IS_ELF_NULL(kernel_elf)) {
        return (Elf_Sym){0};
    }

    return elf_find_sym(&kernel_elf, addr);
}

static const char* get_symbol_name(Elf_Sym symbol) {
    if (IS_ELF_NULL(kernel_elf)) {
        return nullptr;
    }

    return elf_get_str(&kernel_elf, ELF_GETATTR(symbol, st_name));
}

static void dump_backtrace(uintptr_t rbp) {
    struct _StackFrame {
        struct _StackFrame* rbp;
        uint64_t rip;
    }* stackframe = (void*)rbp;

    size_t frame = 0;
    for (;;) {
        if (stackframe->rip == 0) {
            break;
        }
        Elf_Sym rip_sym = get_symbol(stackframe->rip);
        if (IS_ELF_NULL(rip_sym)) {
            print$("     * %p\n", stackframe->rip);
        } else {
            const char* sym_name = get_symbol_name(rip_sym);
            print$("     #%d %p (%s+0x%x)\n", frame++, stackframe->rip, sym_name, stackframe->rip - ELF_GETATTR(rip_sym, st_value));
        }
        stackframe = stackframe->rbp;
    }
}

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
        FreelistAllocator alloc = freelist_allocator_create(addrBuffer, 256);

        Result res_buff = alloc.base.alloc(&alloc.base, 17);
        if (res_buff.type != EOK) {
            debug$("Failed to allocate the buffer");
            goto ignore;
        }

        utoa(regs->rip, (char*)unwrap(res_buff), 16);
        Result b64 = b64encode((char*)unwrap(res_buff), (Allocator*)&alloc);
        if (b64.type != EOK) {
            debug$("Failed to encode to b64");
            goto ignore;
        }

        print$("\033]52;c;%s\033\\", (char*)unwrap(b64));
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
    Elf_Sym rip_sym = get_symbol(regs->rip);
    if (IS_ELF_NULL(rip_sym)) {
        print$("    RIP \033[7m%p\033[0m\n\n", regs->rip);
    } else {
        const char* sym_name = get_symbol_name(rip_sym);
        print$("    RIP \033[7m%p\033[0m (%s+0x%x)\n\n", regs->rip, sym_name, regs->rip - ELF_GETATTR(rip_sym, st_value));
    }
    print$("    Backrace:\n");
    dump_backtrace(regs->rbp);
    print$("\n\x1B[0;33m ---------------------------------------------------------------------------------------------------\x1B[0;31m !!!\x1B[0m\n");
}

void interrupt_handler(uint64_t rsp) {
    Regs* regs = (Regs*)rsp;

    if (regs->intno < IRQ0) {
        kpanic(regs);

        for (;;) {
            asm volatile(
                "cli;"
                "hlt;");
        }
    }

    asm volatile("hlt");
}
