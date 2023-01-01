#pragma once

#include <stddef.h>
#include <stdint.h>

char *itoa(int64_t value, char *str, uint64_t base, size_t buf_size);