#pragma once

#include <stddef.h>

/* --- 7.28.2 - Copying functions ------------------------------------------- */

void* memcpy(void* restrict s1, const void* restrict s2, size_t n);

/* --- 7.28.2 - Miscellaneous functions ------------------------------------------- */

void* memset(void* s, int c, size_t n);

size_t strlen(const char* s);
