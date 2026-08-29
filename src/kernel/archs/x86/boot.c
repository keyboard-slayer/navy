#include <logging.h>
#include <multiboot2.h>

#include "../x86-common/cpuid.h"
#include "../x86-common/e9.h"
#include "../x86-common/gdt.h"
#include "idt.h"

Result get_kernel_elf(void) {
    return err$(ENOENT);
}

noreturn void boot_multiboot2(unsigned long magic, uintptr_t addr) {
    logging_set_stream(e9_writer());

    if (magic != MULTIBOOT2_BOOTLOADER_MAGIC) [[clang::unlikely]] {
        panic$("Invalid multiboot2 magic");
    }

    gdt_setup();
    idt_setup();

    if (!cpuid_long_mode_available()) {
        panic$("Navy doesn't support x86 32bits");
    }

    __asm__ volatile("int $1");

    debug$("Sup grub ?");
    for (;;) {
    }
}
