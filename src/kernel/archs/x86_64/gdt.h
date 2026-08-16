#pragma once

#include <result.h>
#include <stdint.h>

enum gdtaccess : uint8_t {
    GDT_ACCESS_READ_WRITE = (1 << 1),
    GDT_ACCESS_EXE = (1 << 3),
    GDT_ACCESS_DESCRIPTOR = (1 << 4),
    GDT_ACCESS_USER = (3 << 5),
    GDT_ACCESS_PRESENT = (1 << 7),
};

enum gdtflags : uint8_t {
    GDT_FLAGS_LONG_MODE = (1 << 1),
    GDT_FLAGS_SIZE = (1 << 2),
    GDT_FLAGS_GRANULARITY = (1 << 3),
};

enum tssflags : uint8_t {
    TSS_FLAGS_PRESENT = (1 << 7),
    TSS_FLAGS_64BITS_AVAILABLE = (0x9),
};

enum : size_t {
    GDT_NULL_DESC,
    GDT_KERNEL_CODE,
    GDT_KERNEL_DATA,
    GDT_USER_DATA,
    GDT_USER_CODE,
    GDT_TSS,

    GDT_ENTRIES_LENGTH
};

typedef struct [[gnu::packed]] {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t base_middle;
    uint8_t access;
    uint8_t limit_high : 4;
    uint8_t flags : 4;
    uint8_t base_high;
} GdtEntry;

typedef struct [[gnu::packed]] {
    uint16_t length;
    uint16_t base_low;
    uint8_t base_mid;
    uint8_t flags1;
    uint8_t flags2;
    uint8_t base_high;
    uint32_t base_upper;
    uint32_t reserved;
} TssEntry;

typedef struct [[gnu::packed]] {
    uint32_t reserved;
    uint64_t rsp[3];
    uint64_t reserved0;
    uint64_t ist[7];
    uint32_t reserved1;
    uint32_t reserved2;
    uint16_t reserved3;
    uint16_t iopb_offset;
} Tss;

typedef struct [[gnu::packed]] {
    uint16_t size;
    uintptr_t offset;
} GdtDescriptor;

typedef struct [[gnu::packed]] {
    GdtEntry entries[GDT_ENTRIES_LENGTH];
    TssEntry tss_entry;
} Gdt;

void gdt_setup(void);
