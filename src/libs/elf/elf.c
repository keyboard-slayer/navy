#include <logging.h>
#include <stdarg.h>
#include <stddefer.h>
#include <string.h>

#include "include/elf.h"

bool elf_is_valid(uint8_t binary[const static 1]) {
    return memcmp(binary, ELFMAG, EI_MAG) == 0;
}

Elf_Ehdr elf_parse(uint8_t binary[const static 1]) {
    if (binary[EI_CLASS] == 1) {
        return (Elf_Ehdr){.is32 = true, ._32 = (Elf32_Ehdr*)binary};
    }

    return (Elf_Ehdr){.is32 = false, ._64 = (Elf64_Ehdr*)binary};
}

static Elf_Shdr elf_get_sh(Elf_Ehdr self[const static 1], size_t idx) {
    uint8_t* buffer = (uint8_t*)self->_32;
    Elf_Shdr ret = {.is32 = self->is32};

    size_t offset = ELF_GETATTR(*self, e_shoff) + (idx * ELF_GETATTR(*self, e_shentsize));

    if (self->is32) {
        ret._32 = (Elf32_Shdr*)(buffer + offset);
    } else {
        ret._64 = (Elf64_Shdr*)(buffer + offset);
    }

    return ret;
}

Elf_Shdr elf_get_section(Elf_Ehdr self[const static 1], char name[const static 1]) {
    Elf_Shdr shstrtab = elf_get_sh(self, ELF_GETATTR(*self, e_shstrndx));
    uintptr_t offset = ELF_GETATTR(shstrtab, sh_offset);
    uint8_t* shstr = (uint8_t*)(offset + (uintptr_t)self->_32);

    for (size_t i = 0; i < ELF_GETATTR(*self, e_shnum); i++) {
        Elf_Shdr sh = elf_get_sh(self, i);
        const char* sh_name = (const char*)&shstr[ELF_GETATTR(sh, sh_name)];
        if (memcmp(sh_name, name, strlen(name)) == 0) {
            return sh;
        }
    }

    warn$("Couldn't find section with name %s", name);
    return (Elf_Shdr){0};
}

Elf_Sym elf_find_sym(Elf_Ehdr self[const static 1], uintptr_t addr) {
    uint8_t* sym = nullptr;
    Elf_Shdr symtab = {0};

    symtab = elf_get_section(self, ".symtab");
    if (IS_ELF_NULL(symtab)) {
        warn$("Couldn't find symtab");
        return (Elf_Sym){0};
    }

    uintptr_t symtab_off = (uintptr_t)ELF_GETATTR(symtab, sh_offset);
    sym = (uint8_t*)(symtab_off + (uintptr_t)self->_32);

    if (sym == nullptr) {
        return (Elf_Sym){0};
    }

    for (size_t off = 0; off < ELF_GETATTR(symtab, sh_size); off += ELF_GETATTR(symtab, sh_entsize)) {
        Elf_Sym symbol = {.is32 = self->is32};
        if (self->is32) {
            symbol._32 = (Elf32_Sym*)(sym + off);
        } else {
            symbol._64 = (Elf64_Sym*)(sym + off);
        }

        if (ST_TYPE(ELF_GETATTR(symbol, st_info)) != STT_FUNC) {
            continue;
        }

        size_t limit = ELF_GETATTR(symbol, st_value) + ELF_GETATTR(symbol, st_size);

        if (ELF_GETATTR(symbol, st_value) <= addr && addr <= limit) {
            return symbol;
        }
    }

    return (Elf_Sym){0};
}

const char* elf_get_str(Elf_Ehdr self[const static 1], size_t index) {
    static uint8_t* str = nullptr;

    if (str == nullptr) {
        Elf_Shdr strtab = elf_get_section(self, ".strtab");
        if (IS_ELF_NULL(strtab)) {
            return nullptr;
        }

        uintptr_t strtab_off = (uintptr_t)ELF_GETATTR(strtab, sh_offset);
        str = (uint8_t*)(strtab_off + (uintptr_t)self->_32);
    }

    return (const char*)&str[index];
}
