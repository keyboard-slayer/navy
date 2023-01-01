#pragma once

#define loop for (;;)
#define noreturn [[gnu::noreturn]]
#define unused [[gnu::unused]]
#define packed [[gnu::packed]]
#define block_return() __builtin_unreachable()
#define align_up$(x, align) (((x) + (align)-1) & ~((align)-1))
#define align_down$(x, align) ((x) & ~((align)-1))

#define kib$(x) ((x)*1024)
#define mib$(x) (kib$(x) * 1024)
#define gib$(x) (mib$((uint64_t)x) * 1024)

#define max$(a, b) ((a) > (b) ? (a) : (b))
#define min$(a, b) ((a) < (b) ? (a) : (b))