#include <string.h>

/**
 * 7.28.2.1 - memcpy
 * The memcpy function copies n characters from the object pointed to by s2 into the object pointed to
 * by s1. If copying takes place between objects that overlap, the behavior is undefined.
 **/

void* memcpy(void* restrict s1, const void* restrict s2, size_t n) {
    unsigned char* d = s1;
    const unsigned char* s = s2;

    while (n--) {
        *d++ = *s++;
    }

    return s1;
}
