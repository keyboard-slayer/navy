#include <base/maybe.h>
#include <stdint.h>

#define FDT_MAGIC (0xd00dfeed)
#define FDT_BEGIN_NODE (0x00000001)
#define FDT_END_NODE (0x00000002)
#define FDT_PROP (0x00000003)
#define FDT_NOP (0x00000004)
#define FDT_END (0x00000009)

#define ALIGN(x, a) (((x) + ((a)-1)) & ~((a)-1))
#define PALIGN(p, a) ((void *)(ALIGN((unsigned long)(p), (a))))
#define GET_CELL(p) (p += 4, *((const uint32_t *)(p - 4)))

typedef struct
{
    uint32_t magic;
    uint32_t totalsize;
    uint32_t off_dt_struct;
    uint32_t off_dt_strings;
    uint32_t off_mem_rsvmap;
    uint32_t version;
    uint32_t last_comp_version;
    uint32_t boot_cpuid_phys;
    uint32_t size_dt_strings;
    uint32_t size_dt_struct;
} FdtHeader;

typedef struct
{
    uint32_t len;
    uint32_t nameoff;
} FdtProp;

void dtb_register(void *dtb_ptr);
MaybeAddr dtb_find_device_addr_by_name(char const *name);
MaybeAddr dtb_find_compatible_device(char const *name);