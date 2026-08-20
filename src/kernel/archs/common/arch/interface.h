#pragma once

#include <elf.h>
#include <result.h>
#include <stdnoreturn.h>

noreturn void breakpoint(void);

Result get_kernel_elf(void);

uintptr_t hhdm(void);

size_t psize(void);
