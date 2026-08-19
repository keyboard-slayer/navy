#pragma once

#include <stddef.h>

#define __STDC_VERSION_STRING_H__ 202ymmL

/* --- 7.28.2 - Copying functions ------------------------------------------- */

void* memcpy(void* restrict s1, const void* restrict s2, size_t n);

/* --- 7.28.4 - Comparison functions ---------------------------------------- */

int memcmp(const void* s1, const void* s2, size_t n);

/* --- 7.28.6 - Miscellaneous functions ------------------------------------- */

void* memset(void* s, int c, size_t n);

size_t strlen(const char* s);
