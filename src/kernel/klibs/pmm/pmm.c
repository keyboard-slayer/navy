#include <arch/interface.h>
#include <logging.h>

#include "pmm.h"

static Freelist* head = nullptr;
static Freelist* tail = nullptr;
static uintptr_t total_pages = 0;

void pmm_setup(size_t n, MemMap map[const static n]) {
    Freelist* tmp_head = nullptr;

    for (size_t i = 0; i < n; i++) {
        MemMap* m = &map[i];
        if (m->type != MEMMAP_FREE) {
            continue;
        }

        Freelist* node = (Freelist*)(m->addr + hhdm());
        node->next = nullptr;
        node->npages = __builtin_align_down(m->length, psize()) / psize();
        node->base = __builtin_align_up(m->addr, psize());

        total_pages += node->npages;

        if (head == nullptr) {
            head = node;
        }

        if (tmp_head != nullptr) {
            tmp_head->next = node;
        }

        tmp_head = node;
        tail = node;
    }
}

Result pmm_alloc(size_t sz) {
    size_t npage = __builtin_align_up(sz, psize()) / psize();
    Freelist** node = &head;

    while (*node != nullptr) {
        if ((*node)->npages >= npage) {
            uintptr_t ptr = (*node)->base;
            size_t remain = (*node)->npages - npage;

            if (remain > 0) {
                *node = (Freelist*)(ptr + (npage * psize()) + hhdm());
                (*node)->base = ((uintptr_t)*node);
                (*node)->npages = remain;
                total_pages -= remain;
            } else {
                *node = (*node)->next;
            }

            return uok$(ptr);
        }

        node = &(*node)->next;
    }

    return err$(ENOMEM);
}

void pmm_free(void* ptr, size_t sz) {
    if (sz % psize() > 0) {
        panic$("Pointer is not page aligned");
    }

    Freelist* f = (Freelist*)ptr;
    f->base = (uintptr_t)ptr - hhdm();
    f->npages = sz / psize();
    total_pages += sz / psize();
    tail->next = f;
    tail = f;
}
