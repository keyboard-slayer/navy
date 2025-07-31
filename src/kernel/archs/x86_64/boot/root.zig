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
const logger = @import("logger");
const handover = @import("handover");

const limine = @import("./limine.zig");
const as = @import("../asm.zig");
const serial = @import("../serial.zig");
const simd = @import("../simd.zig");
const archSetup = @import("../root.zig").setup;

const kernelUtils = @import("root").kernelUtils;
const main = @import("root").main;

const log = std.log.scoped(.boot);

pub var payload: *handover.Payload = undefined;

pub fn bootPayload() *handover.Payload {
    return payload;
}

pub export fn _start(magic: usize, arg1: usize) callconv(.c) noreturn {
    simd.setupSSE();

    var s = serial.Serial.init() catch unreachable;
    logger.setGlobalWriter(s.writer()) catch unreachable;

    payload = switch (magic) {
        0xc001b001 => @ptrFromInt(arg1),
        else => limine.apply() catch |e| {
            log.err("failed to apply boot protocol {any}", .{e});
            as.hlt();
        },
    };

    if (payload.magic != @intFromEnum(handover.Tags.MAGIC)) {
        log.err("Invalid handover payload", .{});
        as.hlt();
    }

    const agent: [*:0]u8 = @ptrFromInt(@intFromPtr(payload) + payload.agent);
    log.debug("Booted using {s}", .{agent});

    kernelUtils.handover.dumpMmap();

    archSetup() catch |e| {
        std.log.err("Couldn't initiate arch {any}", .{e});
        as.hlt();
    };

    main() catch |e| {
        std.log.err("Kernel panic: {any}", .{e});
    };

    as.hlt();
}
