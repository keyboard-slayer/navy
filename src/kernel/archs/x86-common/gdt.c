#include <logging.h>
#include <string.h>

#include "gdt.h"

static void gdt_entry_init(GdtEntry self[static 1], uint32_t base, uint32_t limit, uint8_t access, uint8_t flags) {
    memset_inline(self, 0, sizeof(GdtEntry));

    self->access = access;
    self->flags = flags;

    self->base_low = base & 0xffff;
    self->base_middle = (base >> 16) & 0xff;
    self->base_high = (base >> 24) & 0xff;

    self->limit_low = limit & 0xffff;
    self->limit_high = (limit >> 16) & 0x0f;
}

static void gdt_lazy_init(GdtEntry self[static 1], uint8_t access, uint8_t flags) {
    gdt_entry_init(self, 0, 0xffffffff, access | GDT_ACCESS_PRESENT | GDT_ACCESS_READ_WRITE | GDT_ACCESS_DESCRIPTOR, flags | GDT_FLAGS_GRANULARITY);
}

static void gdt_flush(GdtDescriptor gdt_desc[const static 1], [[maybe_unused]] Option address) {
#ifdef __BUILDKIT_ARCH_x86_64__
    asm volatile(
        "lgdt (%0);"

        // reload cs
        "push %1;"
        "lea 1f(%%rip), %%rax;"
        "push %%rax;"
        "lretq;"

        // reload ds
        "1:\n"
        "mov %2, %%ax;"
        "mov %%ax, %%ds;"
        "mov %%ax, %%es;" : : "r"((uintptr_t)gdt_desc),
                              "i"(GDT_KERNEL_CODE * 8), "i"(GDT_KERNEL_DATA * 8) : "rax", "memory");
#else
    if (is_none(address)) {
        asm volatile(
            "lgdt (%0);"
            "mov %2, %%ax;"
            "mov %%ax, %%ds;"
            "mov %%ax, %%es;"
            "mov %%ax, %%fs;"
            "mov %%ax, %%gs;"
            "mov %%ax, %%ss;"
            "ljmp %1, $1f;"
            "1:\n"
            : : "r"((uintptr_t)gdt_desc),
                "i"(GDT_KERNEL_CODE * 8), "i"(GDT_KERNEL_DATA * 8)
            : "eax", "memory");
    } else {
        asm volatile(
            "lgdt (%0);"
            "mov %3, %%ax;"
            "mov %%ax, %%ds;"
            "mov %%ax, %%es;"
            "mov %%ax, %%fs;"
            "mov %%ax, %%gs;"
            "mov %%ax, %%ss;"
            "pushl %2;"
            "pushl %1;"
            "lret;"
            : : "r"((uintptr_t)gdt_desc), "r"((uintptr_t)(unwrap(address))),
                "i"(GDT_KERNEL_CODE * 8), "i"(GDT_KERNEL_DATA * 8)
            : "eax", "memory");
    }
#endif
}

void gdt_setup(Gdt gdt[static 1], bool longMode, Option address) {
    GdtDescriptor gdt_desc = {
        .size = sizeof(*gdt) - 1,
        .offset = (uintptr_t)gdt,
    };

    uint8_t codeFlags;
    if (longMode) {
        codeFlags = GDT_FLAGS_LONG_MODE;
    } else {
        codeFlags = GDT_FLAGS_SIZE;
    }

    gdt_lazy_init(&gdt->entries[GDT_KERNEL_CODE], GDT_ACCESS_EXE, codeFlags);
    gdt_lazy_init(&gdt->entries[GDT_KERNEL_DATA], 0, GDT_FLAGS_SIZE);
    gdt_lazy_init(&gdt->entries[GDT_USER_DATA], GDT_ACCESS_USER, GDT_FLAGS_SIZE);
    gdt_lazy_init(&gdt->entries[GDT_USER_CODE], GDT_ACCESS_USER | GDT_ACCESS_EXE, codeFlags);
    gdt_entry_init(&gdt->entries[GDT_TSS], 0, sizeof(TssEntry) - 1, GDT_ACCESS_PRESENT | GDT_ACCESS_EXE | GDT_ACCESS_ACCESSED, 0);

    gdt_flush(&gdt_desc, address);
}
