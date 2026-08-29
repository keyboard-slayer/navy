#include <logging.h>
#include <stddef.h>
#include <string.h>

#include "acpi/table.h"

bool acpi_validate(AcpiHeader header[static const 1], char signature[static const 4]) {
    if (memcmp(header->signature, signature, 4) != 0) {
        debug$("Invalid ACPI signature %s", signature);
        return false;
    }

    uint8_t sum = 0;
    const uint8_t* h = (const uint8_t*)header;

    for (size_t i = 0; i < header->length; i++) {
        sum += h[i];
    }

    return sum == 0;
}
