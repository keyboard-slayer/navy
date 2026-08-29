#pragma once

#include <stdint.h>

typedef struct [[gnu::packed]] {
    char signature[4];
    uint32_t length;
    uint8_t revision;
    uint8_t checksum;
    char OEMID[6];
    char OEMTableID[8];
    uint32_t OEMRevision;
    uint32_t creatorID;
    uint32_t creatorRevision;
} AcpiHeader;

bool acpi_validate(AcpiHeader header[static const 1], char signature[static const 4]);
