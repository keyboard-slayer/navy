#include <arch/interface.h>
#include <base64.h>
#include <elf.h>
#include <logging.h>
#include <pmm.h>
#include <stdint.h>
#include <stdnoreturn.h>
#include <vendor/limine.h>

#include "e9.h"
#include "gdt.h"
#include "idt.h"

[[gnu::used, gnu::section(".limine_requests")]] static volatile uint64_t limine_base_revision[] = LIMINE_BASE_REVISION(6);
[[gnu::used, gnu::section(".limine_requests_start")]] static volatile uint64_t limine_requests_start_marker[] = LIMINE_REQUESTS_START_MARKER;
[[gnu::used, gnu::section(".limine_requests_end")]] static volatile uint64_t limine_requests_end_marker[] = LIMINE_REQUESTS_END_MARKER;
[[gnu::used, gnu::section(".limine_requests")]] static volatile struct limine_memmap_request memmap_req = {.id = LIMINE_MEMMAP_REQUEST_ID};
[[gnu::used, gnu::section(".limine_requests")]] static volatile struct limine_executable_file_request exe_req = {.id = LIMINE_EXECUTABLE_FILE_REQUEST_ID};
[[gnu::used, gnu::section(".limine_requests")]] static volatile struct limine_hhdm_request hhdm_req = {.id = LIMINE_HHDM_REQUEST_ID};

constexpr size_t memmap_count_limit = 64;
static MemMap _memmap[memmap_count_limit] = {0};

uintptr_t hhdm(void) {
    static uintptr_t ret = 0;

    if (ret == 0) {
        if (hhdm_req.response == nullptr) {
            panic$("Couldn't get limine HHDM");
        }

        ret = hhdm_req.response->offset;
    }

    return ret;
}

static void parse_memmap(void) {
    if (memmap_req.response == nullptr) [[clang::unlikely]] {
        panic$("Limine didn't respond to the memory map request");
    }

    if (memmap_req.response->entry_count > memmap_count_limit) [[clang::unlikely]] {
        panic$("The kernel can only support 64 memory map entries");
    }

    for (size_t i = 0; i < memmap_req.response->entry_count; i++) {
        struct limine_memmap_entry* ml = memmap_req.response->entries[i];
        MemMap* m = &_memmap[i];

        switch (ml->type) {
        case LIMINE_MEMMAP_USABLE:
            m->type = MEMMAP_FREE;
            break;

        case LIMINE_MEMMAP_RESERVED:
            [[fallthrough]];
        case LIMINE_MEMMAP_ACPI_NVS:
            [[fallthrough]];
        case LIMINE_MEMMAP_FRAMEBUFFER:
            [[fallthrough]];
        case LIMINE_MEMMAP_RESERVED_MAPPED:
            m->type = MEMMAP_RESERVED;
            break;

        case LIMINE_MEMMAP_ACPI_RECLAIMABLE:
            [[fallthrough]];
        case LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE:
            m->type = MEMMAP_RECLAIMABLE;
            break;

        case LIMINE_MEMMAP_EXECUTABLE_AND_MODULES:
            m->type = MEMMAP_MODULE;
            break;
        }

        m->addr = ml->base;
        m->length = ml->length;
    }
}

Result get_kernel_elf(void) {
    if (exe_req.response == nullptr) [[clang::unlikely]] {
        return err$(ENOENT);
    }

    return uok$((uintptr_t)exe_req.response->executable_file->address);
}

noreturn void kmain(void) {
    logging_set_stream(e9_writer());

    if (!LIMINE_BASE_REVISION_SUPPORTED(limine_base_revision)) [[clang::unlikely]] {
        panic$("This version of limine doesn't support version 6 of the protocol");
    }

    gdt_setup();
    idt_setup();

    parse_memmap();

    pmm_setup(memmap_req.response->entry_count, _memmap);

    log$("Hello, World");
    for (;;)
        ;
}
