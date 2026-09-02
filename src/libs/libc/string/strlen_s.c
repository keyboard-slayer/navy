#if defined(__STDC_WANT_LIB_EXT1__) && __STDC_WANT_LIB_EXT1__ >= 1
#    include <stddef.h>

/*
 * K.3.7.5.4 - strlen_s
 * The strnlen_s function counts not more than maxsize characters (a null character and characters
 * that follow it are not counted) in the array to which s points. At most the first maxsize characters
 * of s shall be accessed by strnlen_s. The implementation shall behave as if it reads the characters
 * sequentially and stops as soon as a null character is found.
 */

size_t strnlen_s(const char* s, size_t maxsize) {
    if (s == nullptr) {
        return 0;
    }

    size_t ret;

    for (ret = 0; s[ret] != '\0' && ret < maxsize; ret++)
        ;

    return ret;
}

#endif
