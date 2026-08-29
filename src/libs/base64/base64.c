#include <stddefer.h>
#include <stdint.h>
#include <string.h>
#include <traits.h>

#include "base64.h"

static char charset[64] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static void toBin(uint8_t n, char* r) {
    memset_inline(r, '0', 8);

    size_t i = 7;
    while (n > 0) {
        r[i--] = n % 2 + '0';
        n /= 2;
    }
}

static size_t fromBin(char n[const static 6]) {
    size_t r = 0;
    size_t p = 32;

    for (size_t i = 0; i < 6; i++) {
        if (n[i] == '1') {
            r += p;
        }

        p /= 2;
    }

    return r;
}

Result b64encode(char s[static const 1], Allocator alloc[static 1]) {
    size_t slen = strlen(s);
    size_t maxLen = ((slen * 4 + 2) / 3) + 2;

    char* result = (char*)try$(alloc->alloc(alloc, maxLen));
    size_t ptr = 0;
    memset(result, 0, maxLen);

    size_t binLen = slen * 8;
    char* binary = (char*)try$(alloc->alloc(alloc, binLen));
    defer alloc->free(alloc, binary, binLen);

    for (size_t i = 0; i < slen; i++) {
        toBin(s[i], binary + (i * 8));
    }

    char subStr[7] = {0};
    size_t i = 0;

    for (i = 0; (binLen - i) >= 6; i += 6) {
        memcpy_inline(subStr, binary + i, 6);
        result[ptr++] = charset[fromBin(subStr)];
    }

    if (binLen - i > 0) {
        size_t padding = 6 - (binLen - i);
        memset_inline(subStr, '0', 6);
        memcpy(subStr, binary + i, (binLen - i));
        result[ptr++] = charset[fromBin(subStr)];

        if (padding >= 2) {
            result[ptr++] = '=';
        }

        if (padding == 4) {
            result[ptr++] = '=';
        }
    }

    result[ptr] = 0;
    return uok$(result);
}
