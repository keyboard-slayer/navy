/*
MIT License

Copyright (c) 2021-2022 The brutal operating system contributors

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

// clang-format off

#pragma once

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <libc/string.h>
#include <base/maybe.h>

/* --- Inline String Buffer ------------------------------------------------- */

typedef struct
{
    size_t len;
    char buf[];
} InlineStr;

/* --- Non Owning Strings --------------------------------------------------- */

typedef struct
{
    size_t len;
    char *buf;
} Str;

#define nullstr str$("")

#define is_nullstr(S) (S.len == 0)

bool str_eq(Str const lhs, Str const rhs);

int str_count(Str const lStr, Str const rStr);

int str_count_chr(Str const str, char chr);

int str_last(Str const lStr, Str const rStr);

int str_last_chr(Str const str, char chr);

int str_first(Str const lStr, Str const rStr);

int str_first_chr(Str const str, char chr);

bool str_any(Str const str);

/* --- Fix Size Strings ----------------------------------------------------- */

#define StrFix(N)    \
    struct           \
    {                \
        size_t len;  \
        char buf[N]; \
    }

typedef StrFix(8) StrFix8;
typedef StrFix(16) StrFix16;
typedef StrFix(32) StrFix32;
typedef StrFix(64) StrFix64;
typedef StrFix(128) StrFix128;

/* --- Cast Between String Types -------------------------------------------- */

static inline Str str_forward(Str str) { return str; }
static inline Str str_make_from_inline_str(InlineStr *str) { return (Str){str->len, str->buf}; }
static inline Str str_make_from_cstr(char const *cstr) { return (Str){strlen(cstr), (char *)cstr}; }
static inline Str str_make_from_cstr8(uint8_t const *cstr) { return (Str){strlen((char *)cstr), (char *)cstr}; }
static inline Str str_make_from_str_fix8(StrFix8 const *str_fix) { return (Str){str_fix->len, (char *)str_fix->buf}; }
static inline Str str_make_from_str_fix16(StrFix16 const *str_fix) { return (Str){str_fix->len, (char *)str_fix->buf}; }
static inline Str str_make_from_str_fix32(StrFix32 const *str_fix) { return (Str){str_fix->len, (char *)str_fix->buf}; }
static inline Str str_make_from_str_fix64(StrFix64 const *str_fix) { return (Str){str_fix->len, (char *)str_fix->buf}; }
static inline Str str_make_from_str_fix128(StrFix128 const *str_fix) { return (Str){str_fix->len, (char *)str_fix->buf}; }

// clang-format off

// Create a new instance of a non owning string.
#define str$(literal)                                \
    _Generic((literal),                              \
        Str              : str_forward,              \
        InlineStr *      : str_make_from_inline_str, \
        char*            : str_make_from_cstr,       \
        char const*      : str_make_from_cstr,       \
        uint8_t*         : str_make_from_cstr8,      \
        uint8_t const*   : str_make_from_cstr8,      \
        StrFix8*         : str_make_from_str_fix8,   \
        StrFix8 const*   : str_make_from_str_fix8,   \
        StrFix16*        : str_make_from_str_fix16,  \
        StrFix16 const*  : str_make_from_str_fix16,  \
        StrFix32*        : str_make_from_str_fix32,  \
        StrFix32 const*  : str_make_from_str_fix32,  \
        StrFix64*        : str_make_from_str_fix64,  \
        StrFix64 const*  : str_make_from_str_fix64,  \
        StrFix128*       : str_make_from_str_fix128, \
        StrFix128 const* : str_make_from_str_fix128  \
    )(literal)

#define str_const$(STR)  (Str){sizeof(STR) - 1, STR}

// clang-format on

#define str_n$(n, str) \
    (Str)              \
    {                  \
        (n), (str)     \
    }

// Create a new instance of a fix size string.
#define str_fix$(T, str)                               \
    ({                                                 \
        T dst_str = {};                                \
        Str src_str = str$(str);                       \
        memcpy(dst_str.buf, src_str.buf, src_str.len); \
        dst_str.len = src_str.len;                     \
        dst_str;                                       \
    })

#define str_sub(str, start, end) \
    str_n$((end) - (start), (char *)str.buf + (start))

typedef Maybe(Str) MaybeStr;