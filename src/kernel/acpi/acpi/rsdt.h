#pragma once

#include <result.h>

#include "rsdp.h"
#include "table.h"

typedef struct {
    AcpiHeader header;
    uintptr_t entries;
} Rsdt;

Result rsdt_load(Rsdp rsdp[static const 1]);
