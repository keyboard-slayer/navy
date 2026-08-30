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
        Result kbin = get_kernel_elf();
        if (kbin.type != EOK) {
            warn$("Couldn't get kernel symbols");
        }

        kernel_elf = elf_parse((uint8_t*)kbin.uvalue);
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

static void dump_backtrace(uintptr_t ebp) {
    struct _StackFrame {
        struct _StackFrame* rbp;
        uint32_t eip;
    }* stackframe = (void*)ebp;

    size_t frame = 0;
    for (;;) {
        if (stackframe->eip < 9) {
            break;
        }
        Elf_Sym eip_sym = get_symbol(stackframe->eip);
        if (IS_ELF_NULL(eip_sym)) {
            print$("     * %p\n", stackframe->eip);
        } else {
            const char* sym_name = get_symbol_name(eip_sym);
            print$("     #%d %p (%s+0x%x)\n", frame++, stackframe->eip, sym_name, stackframe->eip - ELF_GETATTR(eip_sym, st_value));
        }
        stackframe = stackframe->rbp;
    }
}

static void kpanic(Regs regs[const static 1]) {
    uint32_t cr0;
    uint32_t cr2;
    uint32_t cr3;
    uint32_t cr4;

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

        utoa(regs->eip, (char*)unwrap$(res_buff), 16);
        Result b64 = b64encode((char*)unwrap$(res_buff), (Allocator*)&alloc);
        if (b64.type != EOK) {
            debug$("Failed to encode to b64");
            goto ignore;
        }

        print$("\033]52;c;%s\033\\", (char*)unwrap$(b64));
    }

ignore:
    print$("\n\x1B[0;31m!!!\x1B[0;33m ---------------------------------------------------------------------------------------------------\n\n\x1B[0m");
    print$("    KERNEL PANIC\n");
    print$("    %s was raised\n\n", exception_messages[regs->intno]);
    print$("    Registers:\n");
    print$("    interrupt: %x, err: %x \n", regs->intno, regs->err);
    print$("    EAX %p EBX %p ECX %p EDX %p\n", regs->eax, regs->ebx, regs->ecx, regs->edx);
    print$("    ESI %p EDI %p EBP %p ESP %p\n", regs->esi, regs->edi, regs->ebp, regs->esp);
    print$("    CR0 %p CR2 %p CR3 %p CR4 %p\n", cr0, cr2, cr3, cr4);
    print$("    CS  %p FLG %p\n", regs->cs, regs->eflags);
    Elf_Sym eip_sym = get_symbol(regs->eip);
    if (IS_ELF_NULL(eip_sym)) {
        print$("    EIP \033[7m%p\033[0m\n\n", regs->eip);
    } else {
        const char* sym_name = get_symbol_name(eip_sym);
        print$("    EIP \033[7m%p\033[0m (%s+0x%x)\n\n", regs->eip, sym_name, regs->eip - ELF_GETATTR(eip_sym, st_value));
    }
    print$("    Backrace:\n");
    dump_backtrace(regs->ebp);
    print$("\n\x1B[0;33m ---------------------------------------------------------------------------------------------------\x1B[0;31m !!!\x1B[0m\n");
}

void interrupt_handler(uint32_t esp) {
    Regs* regs = (Regs*)esp;

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
