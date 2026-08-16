#include <base64.h>
#include <logging.h>
#include <stdint.h>
#include <stdnoreturn.h>
#include <vendor/limine.h>

#include "e9.h"
#include "gdt.h"
#include "idt.h"

[[gnu::used, gnu::section(".limine_requests")]] static volatile uint64_t limine_base_revision[] = LIMINE_BASE_REVISION(6);
[[gnu::used, gnu::section(".limine_requests_start")]] static volatile uint64_t limine_requests_start_marker[] = LIMINE_REQUESTS_START_MARKER;
[[gnu::used, gnu::section(".limine_requests_end")]] static volatile uint64_t limine_requests_end_marker[] = LIMINE_REQUESTS_END_MARKER;

noreturn void kmain(void) {
    logging_set_stream(e9_writer());

    if (!LIMINE_BASE_REVISION_SUPPORTED(limine_base_revision)) {
        error$("This version of limine doesn't support version 6 of the protocol");
        asm("hlt");
    }

    gdt_setup();
    idt_setup();

    log$("Hello, World");
    for (;;)
        ;
}
