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
const Writer = @import("std").io.AnyWriter;

const LoggingError = error{GlobalWriterAlreadyDefined};
var global_writer: ?Writer = null;

pub fn setGlobalWriter(w: Writer) LoggingError!void {
    if (global_writer != null) {
        return LoggingError.GlobalWriterAlreadyDefined;
    }

    global_writer = w;
}

pub fn print(comptime format: []const u8, args: anytype) void {
    if (global_writer) |w| {
        w.print(format, args) catch {};
    }
}

pub fn log(
    comptime level: std.log.Level,
    comptime scope: @TypeOf(.EnumLiteral),
    comptime format: []const u8,
    args: anytype,
) void {
    if (global_writer) |w| {
        const ansi = comptime switch (level) {
            .debug => "\x1b[34m",
            .info => "\x1b[32m",
            .warn => "\x1b[33m",
            .err => "\x1b[31m",
        };

        const text = comptime switch (level) {
            .debug => "DEBUG",
            .info => "INFO",
            .warn => "WARN",
            .err => "ERROR",
        };

        const prefix = if (scope != .default) @tagName(scope) else "";

        w.print("{s}{s: >6} \x1b[90m{s: <7} \x1b[0m", .{ ansi, text, prefix }) catch {};
        w.print(format, args) catch {};
        w.print("\n", .{}) catch {};
    }
}
