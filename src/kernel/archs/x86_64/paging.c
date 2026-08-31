#include <arch/interface.h>
#include <logging.h>
#include <mm/pmm.h>
#include <result.h>
#include <string.h>

#include "paging.h"

static void* pmroot = nullptr;
static constexpr size_t page_size = 4096;
static size_t pLevel;

extern char limine_start_addr[];
extern char limine_end_addr[];
extern char text_start_addr[];
extern char text_end_addr[];
extern char rodata_start_addr[];
extern char rodata_end_addr[];
extern char data_start_addr[];
extern char data_end_addr[];

size_t paging_level(void) {
    return pLevel;
}

Pmap kernel_pmap(void) {
    return (Pmap){.width = 64, .ptr = pmroot};
}

static Result paging_map_section(uintptr_t base, uintptr_t end, uint8_t flags) {
    size_t aligned_base = __builtin_align_down(base, page_size);
    size_t len = __builtin_align_up(end - base, page_size);

    uintptr_t phys = aligned_base - kaddr_virt() + kaddr_phys();
    return paging_map(kernel_pmap(), aligned_base, phys, len, flags);
}

Result paging_setup(size_t max_level, size_t n, MemMap map[const static n]) {
    pmroot = (void*)unwrap(pmm_alloc(page_size));
    debug$("Kernel page map root: %p", pmroot);
    memset_inline(pmroot, 0, page_size);

    pLevel = max_level;

    try$(paging_map_section((uintptr_t)limine_start_addr, (uintptr_t)limine_end_addr, PAGE_READ));
    try$(paging_map_section((uintptr_t)text_start_addr, (uintptr_t)text_end_addr, PAGE_READ | PAGE_EXECUTE));
    try$(paging_map_section((uintptr_t)rodata_start_addr, (uintptr_t)rodata_end_addr, PAGE_READ));
    try$(paging_map_section((uintptr_t)data_start_addr, (uintptr_t)data_end_addr, PAGE_READ | PAGE_WRITE));

    for (size_t i = 0; i < n; i++) {
        try$(paging_map(kernel_pmap(), map[i].addr + hhdm(), map[i].addr, map[i].length, PAGE_READ | PAGE_WRITE | PAGE_HUGE));
    }

    paging_load(kernel_pmap());

    return Ok();
}
