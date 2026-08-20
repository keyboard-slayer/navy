#include <string.h>

/*
 * 7.28.2.3 - memmove
 * The memmove function copies n characters from the object pointed to by s2 into the object pointed to
 * by s1. Copying takes place as if the n characters from the object pointed to by s2 are first copied
 * into a temporary array of n characters that does not overlap the objects pointed to by s1 and s2, and
 * then the n characters from the temporary array are copied into the object pointed to by s1.
 */

void* memmove(void* s1, const void* s2, size_t n) {
    unsigned char* d = s1;
    const unsigned char* s = s2;

    if (d == s || n == 0) {
        return d;
    }

    if (d < s) {
        for (size_t i = 0; i < n; i++) {
            d[i] = s[i];
        }
    } else {
        for (size_t i = n; i > 0; i--) {
            d[i - 1] = s[i - 1];
        }
    }

    return d;
}
