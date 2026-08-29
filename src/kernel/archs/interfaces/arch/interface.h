#pragma once

#include <elf.h>
#include <result.h>
#include <stdnoreturn.h>

noreturn void breakpoint(void);

Result get_kernel_elf(void);

uintptr_t hhdm(void);

uintptr_t kaddr_phys(void);

uintptr_t kaddr_virt(void);

size_t psize(void);
