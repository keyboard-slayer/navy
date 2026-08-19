#include <string.h>

/*
 * 7.28.4.2 - memcmp
 * The memcmp function compares the first n characters of the object pointed to by s1 to the first n
 * characters of the object pointed to by s2
 */

int memcmp(const void* s1, const void* s2, size_t n) {
    const unsigned char* l = (const unsigned char*)s1;
    const unsigned char* r = (const unsigned char*)s2;

    while (n--) {
        if (*l != *r) {
            return *l - *r;
        }

        l++;
        r++;
    }

    return 0;
}
