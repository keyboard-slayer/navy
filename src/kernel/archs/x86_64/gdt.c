#include <string.h>

#include "gdt.h"

static Gdt gdt = {
    .entries = {
        [0] = {0},
    },
    .tss_entry = {0},
};

static GdtDescriptor gdt_desc = {
    .size = sizeof(gdt) - 1,
    .offset = (uintptr_t)&gdt,
};

static void gdt_lazy_init(GdtEntry self[static 1], uint8_t access, uint8_t flags) {
    memset(self, 0, sizeof(GdtEntry));

    if (access == 0 && flags == 0) {
        return;
    }

    self->access = access | GDT_ACCESS_PRESENT | GDT_ACCESS_READ_WRITE | GDT_ACCESS_DESCRIPTOR;
    self->flags = flags | GDT_FLAGS_GRANULARITY;

    self->base_high = 0;
    self->base_middle = 0;
    self->base_low = 0;

    self->limit_low = 0xffff;
    self->limit_high = 0x0f;
}

static void gdt_flush(void) {
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
        "mov %%ax, %%es;" : : "r"((uintptr_t)&gdt_desc),
                              "i"(GDT_KERNEL_CODE * 8), "i"(GDT_KERNEL_DATA * 8) : "rax", "memory"
    );
}

void gdt_setup(void) {
    gdt_lazy_init(&gdt.entries[GDT_KERNEL_CODE], GDT_ACCESS_EXE, GDT_FLAGS_LONG_MODE);
    gdt_lazy_init(&gdt.entries[GDT_KERNEL_DATA], 0, GDT_FLAGS_SIZE);

    gdt_lazy_init(&gdt.entries[GDT_USER_DATA], GDT_ACCESS_USER, GDT_FLAGS_SIZE);
    gdt_lazy_init(&gdt.entries[GDT_USER_CODE], GDT_ACCESS_USER | GDT_ACCESS_EXE, GDT_FLAGS_LONG_MODE);

    gdt.tss_entry.length = sizeof(TssEntry);
    gdt.tss_entry.flags1 = TSS_FLAGS_PRESENT | TSS_FLAGS_64BITS_AVAILABLE;

    gdt_flush();
}
