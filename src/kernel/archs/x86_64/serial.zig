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

pub const Serial = struct {
    const port: u16 = 0x3f8;

    pub const Error = error{FaultySerialPort};

    pub fn init() !Serial {
        as.out8(port + 1, 0x00);
        as.out8(port + 3, 0x80);
        as.out8(port + 0, 0x03);
        as.out8(port + 1, 0x00);
        as.out8(port + 3, 0x03);
        as.out8(port + 2, 0xc7);
        as.out8(port + 4, 0x0b);
        as.out8(port + 4, 0x1e);
        as.out8(port + 0, 0xae);

        if (as.in8(port + 0) != 0xae) {
            return Error.FaultySerialPort;
        }

        as.out8(port + 4, 0x0f);

        return .{};
    }

    pub fn writer(self: *Serial) std.io.AnyWriter {
        return .{ .context = self, .writeFn = writeOpaque };
    }

    fn writeOpaque(context: *const anyopaque, bytes: []const u8) Error!usize {
        const ptr: *const Serial = @alignCast(@ptrCast(context));
        return write(ptr.*, bytes);
    }

    pub fn write(self: Serial, bytes: []const u8) usize {
        _ = self;

        for (bytes) |b| {
            while (as.in8(port + 5) & 0x20 == 0) {}
            as.out8(port, b);
        }

        return bytes.len;
    }

    fn hasCharacter(self: Serial) u8 {
        _ = self;
        return as.in8(port + 5) & 1;
    }

    pub fn read(self: Serial, alloc: std.mem.Allocator, len: usize) ![]u8 {
        var buffer = try alloc.alloc(u8, len);

        for (0..len) |i| {
            while (self.hasCharacter() == 0) {}
            buffer[i] = as.in8(port);
        }

        return buffer;
    }
};
