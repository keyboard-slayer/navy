#include <string.h>

/*
 * 7.28.6.1 - memset
 * The memset function copies the value of c (converted to an unsigned char) into each of the first n
 * characters of the object pointed to by s.
 */

void* memset(void* s, int c, size_t n) {
    unsigned char* buf = (unsigned char*)s;

    while (n--) {
        *buf++ = (unsigned char)c;
    }

    return s;
}
