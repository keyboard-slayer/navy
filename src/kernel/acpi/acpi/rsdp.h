#pragma once

#include <result.h>
#include <stdint.h>

typedef struct {
    char signature[8];
    uint8_t checksum;
    char OEMID[6];
    uint8_t revision;
    uint32_t rsdtAddress;
} Rsdp;

typedef struct [[gnu::packed]] {
    Rsdp rsdp;

    uint32_t length;
    uint64_t xsdtAddress;
    uint8_t extendedChecksum;
    uint8_t reserved[3];
} Xsdp;

Result rsdp_load(uintptr_t addr);
