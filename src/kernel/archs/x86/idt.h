#pragma once

#include <stdint.h>

#define IDT_ENTRY_COUNT (256)
#define IDT_INT_GATE (0x8E)

typedef struct [[gnu::packed]] {
    uint16_t size;
    uintptr_t offset;
} IdtDescriptor;

typedef struct [[gnu::packed]] {
    uint16_t offset_low;
    uint16_t selector;
    uint8_t zero;
    uint8_t type_attr;
    uint16_t offset_high;
} IdtEntry;

typedef struct [[gnu::packed]] {
    IdtEntry entries[IDT_ENTRY_COUNT];
} Idt;

#define ERR_INT \
    X(8)        \
    X(10)       \
    X(11)       \
    X(12)       \
    X(13)       \
    X(14)       \
    X(17)       \
    X(21)

#define NO_ERR_INT \
    X(1)           \
    X(2)           \
    X(3)           \
    X(4)           \
    X(5)           \
    X(6)           \
    X(7)           \
    X(9)           \
    X(15)          \
    X(16)          \
    X(18)          \
    X(19)          \
    X(20)          \
    X(22)          \
    X(23)          \
    X(24)          \
    X(25)          \
    X(26)          \
    X(27)          \
    X(28)          \
    X(29)          \
    X(30)          \
    X(31)          \
    X(32)          \
    X(33)          \
    X(34)          \
    X(35)          \
    X(36)          \
    X(37)          \
    X(38)          \
    X(39)          \
    X(40)          \
    X(41)          \
    X(42)          \
    X(43)          \
    X(44)          \
    X(45)          \
    X(46)          \
    X(47)

#define COMMON_ASSEMBLY \
    "push %0;"          \
    "pushal;"           \
    "push %%esp;"       \
    "call *%1;"         \
    "mov %%eax, %%esp;" \
    "popal;"            \
    "add $8, %%esp;"    \
    "iret;"

void idt_setup(void);
