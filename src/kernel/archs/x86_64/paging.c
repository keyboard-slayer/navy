#include <arch/interface.h>
#include <logging.h>
#include <string.h>

#include "../x86-common/cpuid.h"
#include "paging.h"
#include "result.h"

static uintptr_t* pmroot = nullptr;
static bool pml5_enabled = false;

static constexpr size_t page_size = 4096;
static constexpr size_t page_1gib = 1'073'741'824;
static constexpr size_t page_2mib = 2'097'152;

extern char limine_start_addr[];
extern char limine_end_addr[];
extern char text_start_addr[];
extern char text_end_addr[];
extern char rodata_start_addr[];
extern char rodata_end_addr[];
extern char data_start_addr[];
extern char data_end_addr[];

Pmap kernel_pmap(void) {
    return (Pmap){._raw = pmroot};
}

static uint64_t paging_translate_flags(uint8_t flags) {
    uint64_t f = X64_PAGE_PRESENT | X64_PAGE_NO_EXECUTE;

    if (flags & PAGE_NONE) {
        return 0;
    }

    if (flags & PAGE_READ) {
    }

    if (flags & PAGE_WRITE) {
        f |= X64_PAGE_WRITABLE;
    }

    if (flags & PAGE_EXECUTE) {
        f &= ~X64_PAGE_NO_EXECUTE;
    }

    if (flags & PAGE_USER) {
        f |= X64_PAGE_USER;
    }

    if (flags & PAGE_HUGE) {
        f |= X64_PAGE_HUGE;
    }

    return f;
}

static Result paging_get_entry(uintptr_t* page, size_t index, bool alloc) {
    if (page[index] & X64_PAGE_PRESENT) {
        return uok$(PAGE_GET_PHYS(page[index]) + hhdm());
    } else if (alloc) {
        uintptr_t* ptr = (uintptr_t*)try$(pmm_alloc(page_size));
        memset_inline(ptr, 0, page_size);
        page[index] = ((uintptr_t)ptr - hhdm()) | X64_PAGE_PRESENT | X64_PAGE_WRITABLE | X64_PAGE_USER;

        return uok$(ptr);
    }
    return err$(ENOENT);
}

static Result paging_map_page(uintptr_t* page, uintptr_t virt, uintptr_t phys, uint64_t flags) {
    size_t pml1_entry = PMLX_GET_INDEX(virt, 0);
    size_t pml2_entry = PMLX_GET_INDEX(virt, 1);
    size_t pml3_entry = PMLX_GET_INDEX(virt, 2);
    size_t pml4_entry = PMLX_GET_INDEX(virt, 3);

    uintptr_t* pml4;
    if (pml5_enabled) {
        size_t pml5 = PMLX_GET_INDEX(virt, 4);
        pml4 = (uintptr_t*)try$(paging_get_entry(page, pml5, true));
    } else {
        pml4 = page;
    }

    uintptr_t* pml3 = (uintptr_t*)try$(paging_get_entry(pml4, pml4_entry, true));
    if (flags & X64_PAGE_HUGE && cpuid_1gb_page_available()) {
        pml3[pml3_entry] = phys | flags;
        return ok$();
    }

    uintptr_t* pml2 = (uintptr_t*)try$(paging_get_entry(pml3, pml3_entry, true));
    if (flags & X64_PAGE_HUGE) {
        pml2[pml2_entry] = phys | flags;
        return ok$();
    }

    uintptr_t* pml1 = (uintptr_t*)try$(paging_get_entry(pml2, pml2_entry, true));
    pml1[pml1_entry] = phys | flags;
    return ok$();
}

static Result paging_map_section(uintptr_t base, uintptr_t end, uint8_t flags) {
    uint64_t x64_flags = paging_translate_flags(flags);
    size_t aligned_base = __builtin_align_down(base, page_size);
    size_t len = __builtin_align_up(end - base, page_size);

    uintptr_t phys = aligned_base - kaddr_virt() + kaddr_phys();

    return paging_map(kernel_pmap(), aligned_base, phys, len, flags);
}

Result paging_map(Pmap page, uint64_t virt, uint64_t phys, size_t len, uint8_t flags) {
    uint64_t x64_flags = paging_translate_flags(flags);
    size_t align = flags & PAGE_HUGE ? (cpuid_1gb_page_available() ? page_1gib : page_2mib) : page_size;

    uint64_t aligned_virt = __builtin_align_down(virt, align);
    uint64_t aligned_phys = __builtin_align_down(phys, align);
    uint64_t aligned_len = __builtin_align_up(len, align);

    for (size_t i = 0; i < aligned_len; i += align) {
        try$(paging_map_page(page._raw, aligned_virt + i, aligned_phys + i, x64_flags));
    }

    return ok$();
}

void paging_load(Pmap page) {
    debug$("Setting cr3 to %p", (uintptr_t)page._raw - hhdm());
    asm volatile("mov %0, %%cr3" ::"r"((uintptr_t)page._raw - hhdm()));
}

Result paging_setup(bool pml5, size_t n, MemMap map[const static n]) {
    pmroot = (uintptr_t*)unwrap$(pmm_alloc(page_size));
    debug$("Kernel page map root: %p", pmroot);
    memset_inline((void*)pmroot, 0, page_size);

    if (pml5) {
        pml5_enabled = true;
        log$("PML5 is supported");
    }

    try$(paging_map_section((uintptr_t)limine_start_addr, (uintptr_t)limine_end_addr, PAGE_READ));
    try$(paging_map_section((uintptr_t)text_start_addr, (uintptr_t)text_end_addr, PAGE_READ | PAGE_EXECUTE));
    try$(paging_map_section((uintptr_t)rodata_start_addr, (uintptr_t)rodata_end_addr, PAGE_READ));
    try$(paging_map_section((uintptr_t)data_start_addr, (uintptr_t)data_end_addr, PAGE_READ | PAGE_WRITE));

    for (size_t i = 0; i < n; i++) {
        try$(paging_map(kernel_pmap(), map[i].addr + hhdm(), map[i].addr, map[i].length, PAGE_READ | PAGE_WRITE | PAGE_HUGE));
    }

    paging_load(kernel_pmap());

    return ok$();
}
