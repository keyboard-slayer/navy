#pragma once

#include <stdint.h>

#define IRQ0 (32)

void interrupt_handler(uint64_t rsp);
