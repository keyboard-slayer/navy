#pragma once

#include <stddef.h>
#include <stdint.h>

#define EI_NIDENT (16)
#define EI_MAG (4)
#define EI_CLASS (4)
#define ELFMAG "\177ELF"

#define __Elf_Ehdr                  \
    struct {                        \
        uint8_t e_ident[EI_NIDENT]; \
        uint16_t e_type;            \
        uint16_t e_machine;         \
        uint16_t e_version;         \
        PTR_T e_entry;              \
        PTR_T e_phoff;              \
        PTR_T e_shoff;              \
        uint32_t flags;             \
        uint16_t e_ehsize;          \
        uint16_t e_phentsize;       \
        uint16_t e_phnum;           \
        uint16_t e_shentsize;       \
        uint16_t e_shnum;           \
        uint16_t e_shstrndx;        \
    }

#define __Elf_Shdr          \
    struct {                \
        uint32_t sh_name;   \
        uint32_t sh_type;   \
        PTR_T sh_flags;     \
        PTR_T sh_addr;      \
        PTR_T sh_offset;    \
        PTR_T sh_size;      \
        uint32_t sh_link;   \
        uint32_t sh_info;   \
        PTR_T sh_addralign; \
        PTR_T sh_entsize;   \
    }

#define PTR_T uint32_t
typedef __Elf_Ehdr Elf32_Ehdr;
typedef __Elf_Shdr Elf32_Shdr;
#undef PTR_T
static_assert(sizeof(Elf32_Ehdr) == 0x34);
static_assert(sizeof(Elf32_Shdr) == 0x28);

#define PTR_T uint64_t
typedef __Elf_Ehdr Elf64_Ehdr;
typedef __Elf_Shdr Elf64_Shdr;
#undef PTR_T
static_assert(sizeof(Elf64_Ehdr) == 0x40);
static_assert(sizeof(Elf64_Shdr) == 0x40);

typedef struct {
    uint32_t st_name;
    uint32_t st_value;
    uint32_t st_size;
    unsigned char st_info;
    unsigned char st_other;
    uint16_t st_shndx;
} Elf32_Sym;

typedef struct {
    uint32_t st_name;
    unsigned char st_info;
    unsigned char st_other;
    uint16_t st_shndx;
    uint64_t st_value;
    uint64_t st_size;
} Elf64_Sym;

#define ST_TYPE(INFO) ((INFO) & 0xf)

typedef enum {
    STT_NOTYPE = 0,
    STT_OBJECT = 1,
    STT_FUNC = 2,
    STT_SECTION = 3,
    STT_FILE = 4,
    STT_COMMON = 5,
    STT_TLS = 6,
    STT_LOOS = 10,
    STT_HIOS = 12,
    STT_LOPROC = 13,
    STT_HIPROC = 15,
} StTypes;

#define ELF_GETATTR(E, A) ((E).is32 ? (E)._32->A : (E)._64->A)
#define ELF_STRUCTS \
    X(Ehdr)         \
    X(Shdr)         \
    X(Sym)

#define X(E)                \
    typedef struct {        \
        bool is32;          \
        union {             \
            Elf32_##E* _32; \
            Elf64_##E* _64; \
        };                  \
    } Elf_##E;
ELF_STRUCTS
#undef X

#define IS_ELF_NULL(E) (E)._32 == nullptr

bool elf_is_valid(uint8_t binary[const static 1]);

Elf_Ehdr elf_parse(uint8_t binary[const static 1]);

Elf_Shdr elf_get_section(Elf_Ehdr self[const static 1], char name[const static 1]);

Elf_Sym elf_find_sym(Elf_Ehdr self[const static 1], uintptr_t addr);

const char* elf_get_str(Elf_Ehdr self[const static 1], size_t index);
