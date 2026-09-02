#include <acpi/rsdp.h>
#include <acpi/rsdt.h>
#include <arch/interface.h>
#include <elf.h>
#include <logging.h>
#include <misc/decorator.h>
#include <mm/pmm.h>
#include <stdint.h>
#include <stdnoreturn.h>
#include <vendor/limine.h>

#include "../x86-common/e9.h"
#include "../x86-common/gdt.h"
#include "idt.h"
#include "paging.h"

noreturn void kmain_limine(void);

static Gdt gdt = {
    .entries = {
        [0] = {0},
    },
};

[[gnu::used, gnu::section(".limine_requests_start")]] static volatile uint64_t limine_requests_start_marker[] = LIMINE_REQUESTS_START_MARKER;
[[gnu::used, gnu::section(".limine_requests_end")]] static volatile uint64_t limine_requests_end_marker[] = LIMINE_REQUESTS_END_MARKER;

[[gnu::used, gnu::section(".limine_requests")]] static volatile uint64_t limine_base_revision[] = LIMINE_BASE_REVISION(6);
[[gnu::used, gnu::section(".limine_requests")]] static volatile struct limine_entry_point_request ep_req = {.id = LIMINE_ENTRY_POINT_REQUEST_ID, .entry = kmain_limine};
[[gnu::used, gnu::section(".limine_requests")]] static volatile struct limine_executable_address_request addr_req = {.id = LIMINE_EXECUTABLE_ADDRESS_REQUEST_ID};
[[gnu::used, gnu::section(".limine_requests")]] static volatile struct limine_memmap_request memmap_req = {.id = LIMINE_MEMMAP_REQUEST_ID};
[[gnu::used, gnu::section(".limine_requests")]] static volatile struct limine_executable_file_request exe_req = {.id = LIMINE_EXECUTABLE_FILE_REQUEST_ID};
[[gnu::used, gnu::section(".limine_requests")]] static volatile struct limine_hhdm_request hhdm_req = {.id = LIMINE_HHDM_REQUEST_ID};
[[gnu::used, gnu::section(".limine_requests")]] static volatile struct limine_rsdp_request rsdp_req = {.id = LIMINE_RSDP_REQUEST_ID};
[[gnu::used, gnu::section(".limine_requests")]] static volatile struct limine_paging_mode_request paging_req = {
    .id = LIMINE_PAGING_MODE_REQUEST_ID,
    .mode = LIMINE_PAGING_MODE_X86_64_5LVL,
    .min_mode = LIMINE_PAGING_MODE_X86_64_4LVL,
    .max_mode = LIMINE_PAGING_MODE_X86_64_5LVL};

constexpr size_t memmap_count_limit = 64;
static MemMap _memmap[memmap_count_limit] = {0};

CACHE(uintptr_t, hhdm) {
    if (hhdm_req.response == nullptr) {
        panic$("Couldn't get limine HHDM");
    }

    return hhdm_req.response->offset;
}

CACHE(uintptr_t, kaddr_phys) {
    if (addr_req.response == nullptr) {
        panic$("Couldn't get kernel address");
    }

    return addr_req.response->physical_base;
}

CACHE(uintptr_t, kaddr_virt) {
    if (addr_req.response == nullptr) {
        panic$("Couldn't get kernel address");
    }

    return addr_req.response->virtual_base;
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

Option get_kernel_elf(void) {
    if (exe_req.response == nullptr) [[clang::unlikely]] {
        return None;
    }

    return Some(exe_req.response->executable_file->address);
}

[[gnu::weak]] noreturn void kmain(void) {
    for (;;)
        ;
}

noreturn void kmain_limine(void) {
    logging_set_stream(e9_writer());

    if (!LIMINE_BASE_REVISION_SUPPORTED(limine_base_revision)) [[clang::unlikely]] {
        panic$("This version of limine doesn't support version 6 of the protocol");
    }

    if (rsdp_req.response == nullptr) [[clang::unlikely]] {
        panic$("Couldn't get RSDP address from limine");
    }

    if (paging_req.response == nullptr) [[clang::unlikely]] {
        panic$("Couldn't get limine's paging mode");
    }

    gdt_setup(&gdt, true, None);
    idt_setup();

    parse_memmap();

    unwrap(pmm_setup(memmap_req.response->entry_count, _memmap));
    unwrap(paging_setup(paging_req.response->mode == LIMINE_PAGING_MODE_X86_64_5LVL ? 5 : 4, memmap_req.response->entry_count, _memmap));

    Rsdp* rsdp = (Rsdp*)unwrap(rsdp_load((uintptr_t)rsdp_req.response->address));
    unwrap(rsdt_load(rsdp));

    log$("Hello, World");

    for (;;)
        ;
}
