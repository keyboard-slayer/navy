#pragma once

#include <errno.h>
#include <stddef.h>

#define __STDC_VERSION_STRING_H__ 202ymmL

/* --- 7.28.2 - Copying functions ------------------------------------------- */

#define memcpy_inline __builtin_memcpy_inline
void* memcpy(void* restrict s1, const void* restrict s2, size_t n);

void* memmove(void* s1, const void* s2, size_t n);

/* --- 7.28.4 - Comparison functions ---------------------------------------- */

int memcmp(const void* s1, const void* s2, size_t n);

/* --- 7.28.6 - Miscellaneous functions ------------------------------------- */

#define memset_inline __builtin_memset_inline
void* memset(void* s, int c, size_t n);

size_t strlen(const char* s);

#if defined(__STDC_WANT_LIB_EXT1__) && __STDC_WANT_LIB_EXT1__ >= 1
#    define __STDC_LIB_EXT1__

/* --- K3.7.2 - Copying functions ------------------------------------------- */

errno_t strncpy_s(char* restrict s1, rsize_t s1max, const char* restrict s2, rsize_t n);

/* --- K.3.7.5 - Miscellaneous functions ------------------------------------ */

size_t strnlen_s(const char* s, size_t maxsize);

#endif // !__STDC_WANT_LIB_EXT1__
