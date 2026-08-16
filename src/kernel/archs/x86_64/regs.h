#pragma once

#include <stdint.h>

#define REGS_READ(REG) asm volatile("mov %%" #REG ", %0" : "=r"((REG)))

#define REGS_PUSHA \
    "push %%rax;"  \
    "push %%rbx;"  \
    "push %%rcx;"  \
    "push %%rdx;"  \
    "push %%rsi;"  \
    "push %%rdi;"  \
    "push %%rbp;"  \
    "push %%r8;"   \
    "push %%r9;"   \
    "push %%r10;"  \
    "push %%r11;"  \
    "push %%r12;"  \
    "push %%r13;"  \
    "push %%r14;"  \
    "push %%r15;"

#define REGS_POPA \
    "pop %%r15;"  \
    "pop %%r14;"  \
    "pop %%r13;"  \
    "pop %%r12;"  \
    "pop %%r11;"  \
    "pop %%r10;"  \
    "pop %%r9;"   \
    "pop %%r8;"   \
    "pop %%rbp;"  \
    "pop %%rdi;"  \
    "pop %%rsi;"  \
    "pop %%rdx;"  \
    "pop %%rcx;"  \
    "pop %%rbx;"  \
    "pop %%rax;"

typedef struct [[gnu::packed]] {
    uint64_t r15;
    uint64_t r14;
    uint64_t r13;
    uint64_t r12;
    uint64_t r11;
    uint64_t r10;
    uint64_t r9;
    uint64_t r8;
    uint64_t rbp;
    uint64_t rdi;
    uint64_t rsi;
    uint64_t rdx;
    uint64_t rcx;
    uint64_t rbx;
    uint64_t rax;

    uint64_t intno;
    uint64_t err;

    uint64_t rip;
    uint64_t cs;
    uint64_t rflags;
    uint64_t rsp;
    uint64_t ss;
} Regs;
