#pragma once

#include <result.h>
#include <stddef.h>
#include <traits.h>

Result b64encode(char s[static const 1], Allocator alloc[static 1]);
