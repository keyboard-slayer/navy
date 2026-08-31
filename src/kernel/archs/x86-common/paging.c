#include <arch/interface.h>
#include <logging.h>
#include <string.h>

#include "../x86-common/cpuid.h"
#include "paging.h"
#include "result.h"

static constexpr size_t page_size = 4096;
static constexpr size_t page_1gib = 1'073'741'824;
static constexpr size_t page_2mib = 2'097'152;

size_t paging_level(void);

static size_t paging_translate_flags(uint8_t flags) {
    size_t f = X64_PAGE_PRESENT | X64_PAGE_NO_EXECUTE;

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

static Result paging_get_entry(Pmap page, size_t index, bool alloc) {
    if (pmap_read(&page, index) & X64_PAGE_PRESENT) {
        return Ok(PAGE_GET_PHYS(pmap_read(&page, index)) + hhdm());
    } else if (alloc) {
        uintptr_t* ptr = (uintptr_t*)try$(pmm_alloc(page_size));
        memset_inline(ptr, 0, page_size);
        pmap_write(&page, index, (uint64_t)ptr - hhdm() | X64_PAGE_PRESENT | X64_PAGE_WRITABLE | X64_PAGE_USER);
        return Ok(ptr);
    }
    return Err(ENOENT);
}

static Result paging_map_page(Pmap page, uintptr_t virt, uintptr_t phys, size_t flags) {
    size_t end = ((flags & X64_PAGE_HUGE) > 0)
                     ? ((paging_level() > 3 && cpuid_1gb_page_available()) ? 2 : 1)
                     : 0;

    Pmap current = page;
    for (size_t lvl = paging_level() - 1; lvl > end; lvl--) {
        size_t index = PMLX_GET_INDEX(page.width, virt, lvl);
        current.ptr = (void*)try$(paging_get_entry(current, index, true));
    }

    pmap_write(&current, PMLX_GET_INDEX(page.width, virt, end), phys | flags);
    return Ok();
}

Result paging_map(Pmap page, uintptr_t virt, uintptr_t phys, size_t len, uint8_t flags) {
    size_t x64_flags = paging_translate_flags(flags);
    size_t align = flags & PAGE_HUGE ? (cpuid_1gb_page_available() ? page_1gib : page_2mib) : page_size;

    uint64_t aligned_virt = __builtin_align_down(virt, align);
    uint64_t aligned_phys = __builtin_align_down(phys, align);
    uint64_t aligned_len = __builtin_align_up(len, align);

    for (size_t i = 0; i < aligned_len; i += align) {
        try$(paging_map_page(page, aligned_virt + i, aligned_phys + i, x64_flags));
    }

    return Ok();
}

void paging_load(Pmap page) {
    debug$("Setting cr3 to %p", (uintptr_t)page.ptr - hhdm());
    asm volatile("mov %0, %%cr3" ::"r"((uintptr_t)page.ptr - hhdm()));
}
