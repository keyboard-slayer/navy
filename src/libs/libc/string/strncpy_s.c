#if defined(__STDC_WANT_LIB_EXT1__) && __STDC_WANT_LIB_EXT1__ >= 1
#    include <stdint.h>
#    include <string.h>

/*
 * K3.7.2.4 - strncpy_s
 * The strncpy_s function copies not more than n successive characters (characters that follow a null
 * character are not copied) from the array pointed to by s2 to the array pointed to by s1. If no null
 * character was copied from s2, then s1[n] is set to a null character.
 * All elements following the terminating null character (if any) written by strncpy_s in the array
 * of s1max characters pointed to by s1 take unspecified values when strncpy_s returns a nonzero value.)
 */

errno_t strncpy_s(char* restrict s1, rsize_t s1max, const char* restrict s2, rsize_t n) {
    if (s1 == nullptr || s1max > RSIZE_MAX || s1max == 0) {
        return EINVAL;
    }

    if (s2 == nullptr || n > RSIZE_MAX ||
        (n >= s1max && s1max <= strnlen_s(s2, s1max)) ||
        ((uintptr_t)s1 < ((uintptr_t)s2 + n) && ((uintptr_t)s2 < ((uintptr_t)s1 + s1max)))) {
        s1[0] = '\0';
        return EINVAL;
    }

    rsize_t i;
    for (i = 0; i < n && s2[i] != '\0'; i++) {
        s1[i] = s2[i];
    }

    if (i < n) {
        s1[i] = '\0';
    } else {
        s1[n] = '\0';
    }

    return EOK;
}

#endif
