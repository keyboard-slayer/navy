#include <base/debug.h>
#include <base/macro.h>
#include <base/str.h>
#include <libc/ctype.h>
#include <libc/stdlib.h>
#include <libc/string.h>
#include <stddef.h>

#include "dtb.h"

static FdtHeader *header = NULL;
static size_t struct_offset = 0;
static size_t strings_offset = 0;

void dtb_register(void *dtb_ptr)
{
    header = dtb_ptr;
    struct_offset = __builtin_bswap32(header->off_dt_struct);
    strings_offset = __builtin_bswap32(header->off_dt_strings);
}

static void shift(size_t depth, char *space)
{
    for (size_t i = 0; i < (depth * 2); i += 2)
    {
        space[i] = ' ';
        space[i + 1] = ' ';
    }

    space[depth * 2] = 0;
}

static bool is_string(char const *s, size_t len)
{
    if (len == 0 || s[len - 1] != '\0')
    {
        return false;
    }

    const char *se = s + len;

    while (s < se)
    {
        char const *bak = s;

        while (s < se && *s && isprint(*s))
        {
            s++;
        }

        if (*s != '\0' || s == bak)
        {
            return false;
        }

        s++;
    }

    return true;
}

static MaybeAddr dtb_find_device(char const *name, char const *comp)
{
    uint32_t tag;
    FdtProp prop;
    char const *s;
    bool in_comp = true;
    size_t depth = 0;
    char const *node = (char const *)((uintptr_t)header + struct_offset);

    while ((tag = __builtin_bswap32(GET_CELL(node))))
    {
        switch (tag)
        {

        case FDT_END_NODE:
        {
            depth--;

            if (depth == 0)
            {
                return Nothing(MaybeAddr);
            }

            break;
        }
        case FDT_BEGIN_NODE:
        {
            if (comp != NULL && *s != 0 && memcmp(node, "compatible", 10) == 0)
            {
                node = PALIGN(node + strlen(node) + 1, 4);
                in_comp = true;
                continue;
            }

            s = node;
            node = PALIGN(node + strlen(node) + 1, 4);

            if (name != NULL && *s != 0 && memcmp(strtok((char *)s, "@"), name, strlen(name)) == 0)
            {
                return Just(MaybeAddr, strtoll(strtok(NULL, "@"), NULL, 16));
            }

            depth++;

            break;
        }

        case FDT_PROP:
        {
            prop.len = __builtin_bswap32(GET_CELL(node));
            prop.nameoff = __builtin_bswap32(GET_CELL(node));
            char const *prev_node = node;
            node = PALIGN(node + prop.len, 4);

            if (comp == NULL)
            {
                break;
            }

            if (is_string(prev_node, prop.len) && in_comp)
            {
                char const *data = prev_node;
                do
                {
                    if (memcmp(data, comp, strlen(comp)) == 0)
                    {
                        strtok((char *)s, "@");
                        return Just(MaybeAddr, strtoll(strtok(NULL, "@"), NULL, 16));
                    }
                    data += strlen(data) + 1;
                } while (data < prev_node + prop.len);
            }
        }
        }
    }

    return Nothing(MaybeAddr);
}

MaybeAddr dtb_find_device_addr_by_name(char const *name)
{
    return dtb_find_device(name, NULL);
}

MaybeAddr dtb_find_compatible_device(char const *name)
{
    return dtb_find_device(NULL, name);
}

void dtb_dump(void)
{
    if (header == NULL)
    {
        return;
    }

    FdtProp prop;
    uint32_t tag;
    size_t depth = 0;

    char const *s;
    char space[256] = {0};
    char const *node = (char const *)((uintptr_t)header + struct_offset);
    char const *str = (char const *)((uintptr_t)header + strings_offset);

    println$("// magic:		0x{x}", __builtin_bswap32(header->magic));
    println$("// totalsize:		0x{x}", __builtin_bswap32(header->totalsize));
    println$("// off_dt_struct:	0x{x}", __builtin_bswap32(header->off_dt_struct));
    println$("// off_dt_strings:	0x{x}", __builtin_bswap32(header->off_dt_strings));
    println$("// off_mem_rsvmap:	0x{x}", __builtin_bswap32(header->off_mem_rsvmap));
    println$("// version:		{}", __builtin_bswap32(header->version));
    println$("// last_comp_version:	{}", __builtin_bswap32(header->last_comp_version));
    println$("// boot_cpuid_phys:	0x{x}", __builtin_bswap32(header->boot_cpuid_phys));
    println$("// size_dt_strings:	0x{x}", __builtin_bswap32(header->size_dt_strings));
    println$("// size_dt_struct:	0x{x}", __builtin_bswap32(header->size_dt_struct));

    while ((tag = __builtin_bswap32(GET_CELL(node))))
    {
        switch (tag)
        {
        case FDT_END_NODE:
        {
            depth--;
            shift(depth, space);
            println$("{}]", space);

            if (depth == 0)
            {
                return;
            }

            break;
        }

        case FDT_BEGIN_NODE:
        {
            s = node;
            node = PALIGN(node + strlen(node) + 1, 4);
            shift(depth, space);

            if (*s == '\0')
            {
                println$("/ [");
            }
            else
            {
                println$("{}{} = [", space, s);
            }

            depth++;
            break;
        }

        case FDT_PROP:
        {
            prop.len = __builtin_bswap32(GET_CELL(node));
            prop.nameoff = __builtin_bswap32(GET_CELL(node));

            shift(depth + 1, space);
            println$("{}{}", space, str + prop.nameoff);

            char const *prev_node = node;
            node = PALIGN(node + prop.len, 4);

            shift(depth + 2, space);

            if (is_string(prev_node, prop.len))
            {
                char const *data = prev_node;
                do
                {
                    println$("{}{}", space, data);
                    data += strlen(data) + 1;
                } while (data < prev_node + prop.len);
            }
            else if (prop.len % 4 == 0)
            {
                const uint32_t *cell = (const uint32_t *)prev_node;

                for (size_t i = 0; i < prop.len; i += 4)
                {
                    println$("{}0x{x}", space, __builtin_bswap32(cell[i]));
                }
            }
            else
            {
                for (size_t i = 0; i < prop.len; i++)
                {
                    println$("{}0x{x}", space, *prev_node++);
                }
            }

            break;
        }

        default:
        {
            println$("Unkown tag {}", *(uint32_t *)node);
            loop {}
        }
        }
    }
}