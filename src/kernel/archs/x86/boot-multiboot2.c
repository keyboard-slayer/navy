#include <logging.h>
#include <mm/pmm.h>
#include <multiboot2.h>
#include <stddef.h>
#include <string.h>

#include "../x86-common/cpuid.h"
#include "../x86-common/e9.h"
#include "../x86-common/gdt.h"
#include "idt.h"
#include "paging.h"

static uintptr_t multiboot2_addr;
static uintptr_t hhdm_offset;

constexpr size_t memmap_count_limit = 64;
static MemMap _memmap[memmap_count_limit] = {0};
static size_t memmap_count = 0;

static Gdt gdt = {
    .entries = {
        [0] = {0},
    },
};

static bool multiboot2_filter_module(struct multiboot_tag* tag, void* ctx) {
    const char* cmdline = (const char*)ctx;
    struct multiboot_tag_module* m = (struct multiboot_tag_module*)tag;
    return memcmp(m->cmdline, cmdline, strlen(m->cmdline)) == 0;
}

static Result multiboot2_find(multiboot_uint32_t type, void* ctx, bool (*filter_func)(struct multiboot_tag*, void*)) {
    for (struct multiboot_tag* tag = ((struct multiboot_tag*)multiboot2_addr + 8);
         tag->type != MULTIBOOT_TAG_TYPE_END;
         tag = (struct multiboot_tag*)((multiboot_uint8_t*)tag + ((tag->size + 7) & ~7))) {

        if (tag->type == type) {
            if (filter_func == nullptr || (filter_func != nullptr && filter_func(tag, ctx))) {
                return Ok(tag);
            }
        }
    }

    return Err(ENOENT);
}

Option get_kernel_elf(void) {
    struct multiboot_tag_module* m = (struct multiboot_tag_module*)try$(result_ok(
        multiboot2_find(MULTIBOOT_TAG_TYPE_MODULE, "vmnavy32", multiboot2_filter_module)));

    return Some(m->mod_start);
}

static Result parse_memmap(void) {
    struct multiboot_tag_mmap* mmap = (struct multiboot_tag_mmap*)try$(
        multiboot2_find(MULTIBOOT_TAG_TYPE_MMAP, nullptr, nullptr));

    for (struct multiboot_mmap_entry* entry = mmap->entries;
         (uint8_t*)entry < (uint8_t*)mmap + mmap->size;
         entry = (struct multiboot_mmap_entry*)((uintptr_t)entry + mmap->entry_size)) {
        MemMap* m = &_memmap[memmap_count++];
        switch (entry->type) {
        case MULTIBOOT_MEMORY_AVAILABLE:
            m->type = MEMMAP_FREE;
            break;

        case MULTIBOOT_MEMORY_ACPI_RECLAIMABLE:
            m->type = MEMMAP_RECLAIMABLE;
            break;

        case MULTIBOOT_MEMORY_NVS:
            [[fallthrough]];
        case MULTIBOOT_MEMORY_BADRAM:
            [[fallthrough]];
        case MULTIBOOT_MEMORY_RESERVED:
            [[fallthrough]];
        default:
            m->type = MEMMAP_RESERVED;
            break;
        };

        m->addr = entry->addr;
        m->length = entry->len;
    }

    return Ok();
}

uintptr_t hhdm(void) {
    return hhdm_offset;
}

noreturn void boot_multiboot2(unsigned long magic, uintptr_t addr) {
    multiboot2_addr = addr;
    hhdm_offset = 0;
    logging_set_stream(e9_writer());

    if (magic != MULTIBOOT2_BOOTLOADER_MAGIC) [[clang::unlikely]] {
        panic$("Invalid multiboot2 magic");
    }

    gdt_setup(&gdt, false, None);
    idt_setup();

    unwrap(parse_memmap());
    unwrap(pmm_setup(memmap_count, _memmap));

    if (cpuid_long_mode_available()) {
        unwrap(paging64_setup(memmap_count, _memmap));
    }

    debug$("Sup grub ?");
    for (;;) {
    }
}
