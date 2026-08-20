#pragma once

#define CACHE(T, F)                        \
    static T F##_impl(void);               \
    T F(void) {                            \
        static bool cached = false;        \
        static T ret;                      \
        if (!cached) [[clang::unlikely]] { \
            cached = true;                 \
            ret = F##_impl();              \
        }                                  \
        return ret;                        \
    }                                      \
    static T F##_impl(void)
