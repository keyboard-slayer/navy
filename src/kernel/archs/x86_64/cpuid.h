#pragma once

#include <stdint.h>

#define CPUID_EXTENDED_FEATURES_LEAF (0x80000001)
#define CPUID_EXTENDED_CPU_FEATURES_LEAF (0x7)

typedef struct {
    uint64_t rax;
    uint64_t rbx;
    uint64_t rcx;
    uint64_t rdx;

    bool success;
} CpuidRegs;

bool cpuid_1gb_page_available(void);

bool cpuid_5level_page_available(void);
