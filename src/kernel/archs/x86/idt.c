#include "idt.h"

#include "../x86-common/gdt.h"
#include "int_handler.h"

static Idt idt;
static IdtDescriptor idt_desc = {
    .size = sizeof(idt) - 1,
    .offset = (uintptr_t)&idt,
};

static void idt_init_entry(IdtEntry* self, uint32_t base) {
    self->offset_low = base & 0xFFFF;
    self->offset_high = (base >> 16) & 0xFFFF;
    self->selector = GDT_KERNEL_CODE * 8;
    self->type_attr = IDT_INT_GATE;
    self->zero = 0;
}

// clang-format off
#define X(n)                                                  \
    [[gnu::naked]] static void idt_handler_##n(void) {        \
        asm volatile(                                         \
            "push $0;"                                       \
            COMMON_ASSEMBLY                                   \
            :: "i"((uint32_t)(n)), "r"(interrupt_handler)     \
        );                                                    \
    }
NO_ERR_INT
#undef X

#define X(n)                                                  \
    [[gnu::naked]]static void idt_handler_##n(void) {         \
        asm volatile(                                         \
            COMMON_ASSEMBLY                                   \
            :: "i"((uint32_t)(n)), "r"(interrupt_handler)     \
        );                                                    \
    }
ERR_INT
#undef X
// clang-format on

void idt_setup(void) {
#define X(n) idt_init_entry(&idt.entries[n], (uintptr_t)idt_handler_##n);
    NO_ERR_INT
    ERR_INT
#undef X
    asm volatile("lidt (%0)" ::"r"((uintptr_t)&idt_desc));
}
