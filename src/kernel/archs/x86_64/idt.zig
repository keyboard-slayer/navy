// Navy - An experimental voyage, one wave at a time.
// Copyright (C) 2025   Keyb <contact@keyb.moe>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

const std = @import("std");
const as = @import("./asm.zig");
const GdtType = @import("./gdt.zig").GdtType;
const handler = @import("./intHandler.zig").interrupt_handler;

const log = std.log.scoped(.idt);

const IDT_ENTRY_COUNT: usize = 256;
const IDT_INTERRUPT_PRESENT: usize = (1 << 7);
const IDT_INTERRUPT_GATE: usize = 0xe;

var idt: Idt = std.mem.zeroes(Idt);

const IdtEntry = packed struct(u128) {
    offset_low: u16,
    selector: u16,
    ist: u3,
    reserved0: u5 = 0,
    gate_type: u4,
    zero: u1,
    dpl: u2,
    present: u1,
    offset_middle: u16,
    offset_high: u32,
    reserved1: u32 = 0,

    pub fn init(base: u64, gate_type: u4) IdtEntry {
        return .{
            .ist = 0,
            .zero = 0,
            .present = 1,
            .offset_low = @intCast(base & 0xffff),
            .offset_middle = @intCast((base >> 16) & 0xffff),
            .offset_high = @intCast(base >> 32 & 0xffffffff),
            .selector = @as(u16, @intFromEnum(GdtType.KernelCode)) * 8,
            .gate_type = gate_type,
            .dpl = 0,
        };
    }
};

const Idt = extern struct {
    entries: [IDT_ENTRY_COUNT]IdtEntry,
};

const IdtDescriptor = packed struct {
    size: u16,
    offset: u64,

    pub fn load(_idt: *const Idt) IdtDescriptor {
        return .{
            .size = @intCast(@sizeOf(Idt) - 1),
            .offset = @intFromPtr(_idt),
        };
    }

    pub fn apply(self: *const IdtDescriptor) void {
        asm volatile ("lidt (%[self])"
            :
            : [self] "r" (@intFromPtr(self)),
        );
    }
};

fn isr(comptime intno: u64) fn () callconv(.naked) void {
    const has_err = switch (intno) {
        8, 10...14, 17, 21 => true,
        else => false,
    };

    return struct {
        fn func() callconv(.naked) void {
            if (!has_err) {
                asm volatile ("pushq $0");
            }

            asm volatile (
                \\pushq %[intno]
                \\cld
                \\push %%rax
                \\push %%rbx
                \\push %%rcx
                \\push %%rdx
                \\push %%rsi
                \\push %%rdi
                \\push %%rbp
                \\push %%r8
                \\push %%r9
                \\push %%r10
                \\push %%r11
                \\push %%r12
                \\push %%r13
                \\push %%r14
                \\push %%r15
                \\mov %%rsp, %%rdi
                \\call *%[handler]
                \\mov %%rax, %%rsp
                \\pop %r15
                \\pop %r14
                \\pop %r13
                \\pop %r12
                \\pop %r11
                \\pop %r10
                \\pop %r9
                \\pop %r8
                \\pop %rbp
                \\pop %rdi
                \\pop %rsi
                \\pop %rdx
                \\pop %rcx
                \\pop %rbx
                \\pop %rax
                \\add $16, %%rsp
                \\iretq
                :
                : [intno] "r" (intno),
                  [handler] "r" (@intFromPtr(&handler)),
            );
        }
    }.func;
}

pub fn setup() void {
    inline for (&idt.entries, 0..IDT_ENTRY_COUNT) |*entry, i| {
        entry.* = IdtEntry.init(@intFromPtr(&isr(i)), IDT_INTERRUPT_GATE);
    }

    IdtDescriptor.load(&idt).apply();
    log.debug("IDT initialized successfully.", .{});
}
