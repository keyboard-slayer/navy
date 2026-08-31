#pragma once

#include "../x86-common/paging.h"

Result paging_setup(size_t max_level, size_t n, MemMap map[const static n]);
