#pragma once

#include <stdint.h>

void out8(uint16_t port, uint8_t value);

uint8_t in8(uint16_t port);
