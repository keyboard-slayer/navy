#pragma once

#include <stdint.h>

#define REGS_READ(REG) asm volatile("mov %%" #REG ", %0" : "=r"((REG)))

typedef struct [[gnu::packed]] {
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t esp;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;

    uint32_t intno;
    uint32_t err;

    uint32_t eip;
    uint32_t cs;
    uint32_t eflags;
} Regs;
