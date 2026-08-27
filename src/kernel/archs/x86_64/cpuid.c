#include <logging.h>
#include <misc/decorator.h>

#include "cpuid.h"

CpuidRegs cpuid(uint32_t leaf, uint32_t subleaf) {
    uint64_t cpuid_max;

    asm volatile("cpuid"
                 : "=a"(cpuid_max)
                 : "a"(leaf & 0x80000000)
                 : "rbx", "rcx", "rdx");

    if (leaf > cpuid_max) {
        return (CpuidRegs){.success = false};
    }

    CpuidRegs ret = {.success = true};
    asm volatile("cpuid"
                 : "=a"(ret.rax), "=b"(ret.rbx), "=c"(ret.rcx),
                   "=d"(ret.rdx)
                 : "a"(leaf), "c"(subleaf));
    return ret;
}

CACHE(bool, cpuid_1gb_page_available) {
    CpuidRegs regs = cpuid(CPUID_EXTENDED_FEATURES_LEAF, 0);
    constexpr uint64_t pdpe1gb_off = 1 << 26;
    bool ret = !regs.success ? false : regs.rdx & pdpe1gb_off;

    if (ret) {
        debug$("1Gb pages are available");
    }

    return ret;
}

CACHE(bool, cpuid_5level_page_available) {
    CpuidRegs regs = cpuid(CPUID_EXTENDED_CPU_FEATURES_LEAF, 0);
    constexpr uint64_t la57_off = 1 << 16;
    bool ret = !regs.success ? false : regs.rcx & la57_off;
    if (ret) {
        debug$("5 Level paging is available");
    }

    return ret;
}
