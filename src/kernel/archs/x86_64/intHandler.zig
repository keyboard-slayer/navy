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

const logger = @import("logger");
const std = @import("std");

const Serial = @import("./serial.zig").Serial;
const Registers = @import("./regs.zig").Registers;
const as = @import("./asm.zig");

const log = std.log.scoped(.int);

const INT_BREAKPOINT = 3;
const INT_PAGEFAULT = 0xe;

const exception_message: [32][]const u8 = .{
    "Division By Zero",
    "Debug",
    "Non Maskable Interrupt",
    "Breakpoint",
    "Detected Overflow",
    "Out Of Bounds",
    "Invalid Opcode",
    "No Coprocessor",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Bad Tss",
    "Segment Not Present",
    "StackFault",
    "General Protection Fault",
    "Page Fault",
    "Unknown Interrupt",
    "Coprocessor Fault",
    "Alignment Check",
    "Machine Check",
    "SIMD Floating-Point Exception",
    "Virtualization Exception",
    "Control Protection Exception",
    "Reserved",
    "Hypervisor Injection Exception",
    "paging Communication Exception",
    "Security Exception",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
};

fn dumpRegs(regs: *Registers) void {
    var backtrace = std.debug.StackIterator.init(@returnAddress(), null);
    defer backtrace.deinit();

    logger.print("\n!!! ---------------------------------------------------------------------------------------------------\n\n", .{});
    if (regs.intno != INT_BREAKPOINT) {
        logger.print("    KERNEL PANIC\n\n", .{});
        logger.print("    {s} was raised\n", .{exception_message[regs.intno]});
        logger.print("    interrupt: {x}, err: {x}\n\n", .{ regs.intno, regs.err });
    } else {
        logger.print("    BREAKPOINT\n\n", .{});
    }

    logger.print("    RAX {x:0>16} RBX {x:0>16} RCX {x:0>16} RDX {x:0>16}\n", .{ regs.rax, regs.rbx, regs.rcx, regs.rdx });
    logger.print("    RSI {x:0>16} RDI {x:0>16} RBP {x:0>16} RSP {x:0>16}\n", .{ regs.rsi, regs.rdi, regs.rbp, regs.rsp });
    logger.print("    R8  {x:0>16} R9  {x:0>16} R10 {x:0>16} R11 {x:0>16}\n", .{ regs.r8, regs.r9, regs.r10, regs.r11 });
    logger.print("    R12 {x:0>16} R13 {x:0>16} R14 {x:0>16} R15 {x:0>16}\n", .{ regs.r12, regs.r13, regs.r14, regs.r15 });
    logger.print("    CR0 {x:0>16} CR2 {x:0>16} CR3 {x:0>16} CR4 {x:0>16}\n", .{ as.cr0.read(), as.cr2.read(), as.cr3.read(), as.cr4.read() });
    logger.print("    CS  {x:0>16} SS  {x:0>16} FLG {x:0>16}\n", .{ regs.cs, regs.ss, regs.rflags });
    logger.print("    RIP \x1B[7m{x:0>16}\x1B[0m ", .{regs.rip});

    logger.print("\n    Backtrace:\n\n", .{});

    while (backtrace.next()) |address| {
        if (address == 0) {
            break;
        }

        logger.print("    * 0x{x:0>16}\n", .{address});
    }

    logger.print("\n--------------------------------------------------------------------------------------------------- !!!\n\n", .{});
}

pub export fn interrupt_handler(rsp: u64) u64 {
    const regs = Registers.fromRsp(rsp);

    if (regs.intno == INT_BREAKPOINT) {
        dumpRegs(regs);

        var buffer: [8]u8 = std.mem.zeroes([8]u8);
        var allocator = std.heap.FixedBufferAllocator.init(&buffer);
        const alloc = allocator.allocator();

        logger.print("\n\nPress any key to continue...\n", .{});

        const s = Serial{};
        _ = s.read(alloc, 1) catch |err| {
            log.err("Failed to read from serial port: {any}\n", .{err});
            @panic("Failed to read from serial port");
        };
        return rsp;
    } else if (regs.intno < exception_message.len) {
        dumpRegs(regs);
        as.hlt();
    } else {
        const irq = regs.intno - exception_message.len;
        if (irq == 0) {} else {}
    }

    as.hlt();
    return rsp;
}
