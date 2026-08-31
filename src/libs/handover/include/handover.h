#pragma once

#include <result.h>
#include <stdint.h>
#include <stdnoreturn.h>
#include <traits.h>

#ifdef __BUILDKIT_ARCH_x86__
constexpr uintptr_t KERNEL_BASE = 0xc0000000;
constexpr uintptr_t UPPER_HALF = 0xc0000000;
#elif __BUILDKIT_ARCH_x86_64__
constexpr uintptr_t KERNEL_BASE = 0xffffffff80000000;
constexpr uintptr_t UPPER_HALF = 0xffff800000000000;
#endif

#define HANDOVER_TAGS       \
    X(FREE, 0x00000000)     \
    X(MAGIC, 0xc001b001)    \
    X(SELF, 0xa24f988d)     \
    X(STACK, 0xf65b391b)    \
    X(KERNEL, 0xbfc71b20)   \
    X(LOADER, 0xf1f80c26)   \
    X(FILE, 0xcbc36d3b)     \
    X(RSDP, 0x8ef29c18)     \
    X(FDT, 0xb628bbc1)      \
    X(FB, 0xe2d55685)       \
    X(CMDLINE, 0x435140c4)  \
    X(RESERVED, 0xb8841d2d) \
    X(END, 0xffffffff)

typedef enum : uint32_t {

#define X(NAME, VAL) HANDOVER_##NAME = VAL,
    HANDOVER_TAGS
#undef X
} HandoverTag;

static char* const handover_tag_name[] = {
#define X(NAME, _) #NAME,
    HANDOVER_TAGS
#undef X
};

typedef enum : uint16_t {
    RGBX8888 = 0x7451,
    BGRX8888 = 0xd040,
} PixelFormat;

typedef struct {
    HandoverTag tag;
    uint32_t flag;
    uint64_t start;
    uint64_t size;

    union {
        struct {
            uint16_t width;
            uint16_t height;
            uint16_t pitch;
            PixelFormat format;
        } fb;

        struct {
            uint32_t name;
            uint32_t meta;
        } blob;

        uint64_t more;
    };
} HandoverRecord;

typedef struct {
    uint32_t magic;
    uint32_t agent;
    uint32_t size;
    uint32_t len;

    HandoverRecord records[];
} HandoverPayload;

typedef struct
{
    uint32_t tag;
    uint32_t flags;
    uint64_t more;
} HandoverRequest;

typedef void (*HandoverEntryPoint)(uint32_t magic, const HandoverPayload* payload);

/* --- Builder -------------------------------------------------------------- */

typedef struct {
    Allocator* alloc;
} HandoverBuilder;

HandoverBuilder handover_builder_create(Allocator alloc[static 1]);

Result handover_builder_append(HandoverBuilder self[static 1], HandoverRecord record);
