#pragma once

#include <stdint.h>

#define CPUID_EXTENDED_FEATURES_LEAF (0x80000001)
#define CPUID_EXTENDED_CPU_FEATURES_LEAF (0x7)

typedef struct {
    uint32_t eax;
    uint32_t ebx;
    uint32_t ecx;
    uint32_t edx;

    bool success;
} CpuidRegs;

bool cpuid_1gb_page_available(void);

bool cpuid_5level_page_available(void);

bool cpuid_long_mode_available(void);
