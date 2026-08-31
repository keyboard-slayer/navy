#include <arch/interface.h>
#include <logging.h>
#include <mm/pmm.h>
#include <string.h>

#include "../x86-common/cpuid.h"
#include "../x86-common/gdt.h"
#include "../x86-common/paging.h"
#include "paging.h"

void* pmroot = nullptr;
static constexpr size_t page_size = 4096;
static uintptr_t pLevel;

extern char text_start_addr[];
extern char text_end_addr[];
extern char rodata_start_addr[];
extern char rodata_end_addr[];
extern char data_start_addr[];
extern char data_end_addr[];

size_t paging_level(void) {
    return pLevel;
}

static void paging_enable_paging32(void) {
    constexpr uint32_t pg_offset = (1LL << 31) | (1LL << 0);

    uint32_t cr0;
    asm volatile("mov %%cr0, %0" : "=r"(cr0));
    cr0 |= pg_offset;
    asm volatile("mov %0, %%cr0" ::"r"(cr0));
}

static void paging_disable_paging32(void) {
    constexpr uint32_t pg_offset = 1LL << 31;

    uint32_t cr0;
    asm volatile("mov %%cr0, %0" : "=r"(cr0));
    cr0 &= ~pg_offset;
    asm volatile("mov %0, %%cr0" ::"r"(cr0));
}

static void paging_enable_pml5(void) {
    constexpr uint32_t la57_offset = 1LL << 12;

    uint32_t cr4;
    asm volatile("mov %%cr4, %0" : "=r"(cr4));
    cr4 |= la57_offset;
    asm volatile("mov %0, %%cr4" ::"r"(cr4));
}

static void paging_enable_pae(void) {
    constexpr uint32_t pae_offset = 1LL << 5;

    uint32_t cr4;
    asm volatile("mov %%cr4, %0" : "=r"(cr4));
    cr4 |= pae_offset;
    asm volatile("mov %0, %%cr4" ::"r"(cr4));
}

static void paging_enable_lme(void) {
    constexpr uint64_t lme_offset = 1 << 8;

    uint64_t efer;
    asm volatile("rdmsr" : "=A"(efer) : "c"(0xC0000080));
    efer |= lme_offset;
    asm volatile("wrmsr" : : "c"(0xC0000080), "A"(efer));
}

static void paging_enable_pse(void) {
    constexpr uint32_t pse_offset = 1LL << 4;

    uint32_t cr4;
    asm volatile("mov %%cr4, %0" : "=r"(cr4));
    cr4 |= pse_offset;
    asm volatile("mov %0, %%cr4" ::"r"(cr4));
}

static Result paging_map_section(uintptr_t base, uintptr_t end, uint8_t flags, size_t width) {
    size_t aligned_base = __builtin_align_down(base, page_size);
    size_t len = __builtin_align_up(end - base, page_size);

    return paging_map((Pmap){.ptr = pmroot, .width = width}, aligned_base, base, len, flags);
}

static Gdt gdt64 = {
    .entries = {
        [0] = {0},
    },
};

Result paging64_setup(size_t n, MemMap map[const static n]) {
    paging_disable_paging32();
    pmroot = (void*)unwrap(pmm_alloc(page_size));
    memset_inline(pmroot, 0, page_size);
    debug$("Kernel page map root: %p", pmroot);

    if (cpuid_5level_page_available()) {
        pLevel = 5;
        paging_enable_pml5();
    } else {
        pLevel = 4;
    }

    try$(paging_map_section((uintptr_t)text_start_addr, (uintptr_t)text_end_addr, PAGE_READ | PAGE_EXECUTE, 64));
    try$(paging_map_section((uintptr_t)rodata_start_addr, (uintptr_t)rodata_end_addr, PAGE_READ, 64));
    try$(paging_map_section((uintptr_t)data_start_addr, (uintptr_t)data_end_addr, PAGE_READ | PAGE_WRITE, 64));

    for (size_t i = 0; i < n; i++) {
        try$(paging_map((Pmap){.ptr = pmroot, .width = 64}, map[i].addr, map[i].addr, map[i].length, PAGE_READ | PAGE_WRITE | PAGE_HUGE));
    }

    paging_enable_pae();
    paging_enable_lme();
    paging_load((Pmap){.ptr = pmroot, .width = 64});
    paging_enable_paging32();
    gdt_setup(&gdt64, true, Some(0xdeadbeef));
    paging_enable_pse();

    return Ok();
}
