#pragma once

#include "../x86-common/paging.h"

#define PAGING_KERNEL_BASE32 (0xc0000000)
#define PAGING_UPPER_HALF32 (0xc0000000)

#define PAGING_KERNEL_BASE64 (0xffffffff80000000)
#define PAGING_UPPER_HALF64 (0xffff800000000000)

Result paging64_setup(size_t n, MemMap map[const static n]);
