#include <string.h>

/*
 * 7.28.6.4 - strlen
 * The strlen function computes the length of the string pointed to by s.
 */

size_t strlen(const char* s) {
    size_t ret;

    for (ret = 0; s[ret] != '\0'; ret++)
        ;

    return ret;
}
