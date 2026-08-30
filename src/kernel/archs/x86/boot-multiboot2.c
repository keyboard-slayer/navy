#include <logging.h>
#include <multiboot2.h>
#include <string.h>

#include "../x86-common/cpuid.h"
#include "../x86-common/e9.h"
#include "../x86-common/gdt.h"
#include "idt.h"

static uintptr_t multiboot2_addr;

static bool multiboot2_filter_module(struct multiboot_tag* tag, void* ctx) {
    const char* cmdline = (const char*)ctx;
    struct multiboot_tag_module* m = (struct multiboot_tag_module*)tag;
    return memcmp(m->cmdline, cmdline, strlen(m->cmdline)) == 0;
}

static Result multiboot2_find(multiboot_uint32_t type, void* ctx, bool (*filter_func)(struct multiboot_tag*, void*)) {
    unsigned* size = (unsigned*)multiboot2_addr;
    for (struct multiboot_tag* tag = ((struct multiboot_tag*)multiboot2_addr + 8);
         tag->type != MULTIBOOT_TAG_TYPE_END;
         tag = (struct multiboot_tag*)((multiboot_uint8_t*)tag + ((tag->size + 7) & ~7))) {

        if (tag->type == type && filter_func(tag, ctx)) {
            return uok$(tag);
        }
    }

    return err$(ENOENT);
}

Result get_kernel_elf(void) {
    struct multiboot_tag_module* m = (struct multiboot_tag_module*)unwrap$(multiboot2_find(MULTIBOOT_TAG_TYPE_MODULE, "vmnavy32", multiboot2_filter_module));
    return uok$(m->mod_start);
}

noreturn void boot_multiboot2(unsigned long magic, uintptr_t addr) {
    multiboot2_addr = addr;
    logging_set_stream(e9_writer());

    if (magic != MULTIBOOT2_BOOTLOADER_MAGIC) [[clang::unlikely]] {
        panic$("Invalid multiboot2 magic");
    }

    gdt_setup();
    idt_setup();

    if (!cpuid_long_mode_available()) {
        panic$("Navy doesn't support x86 32bits");
    }

    __asm__ volatile("int $3");

    debug$("Sup grub ?");
    for (;;) {
    }
}
