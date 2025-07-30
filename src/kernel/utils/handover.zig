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
const handover = @import("handover");
const boot = @import("arch").boot;

pub fn dumpMmap() void {
    std.log.debug("+-------------------------------------------------------------------+", .{});
    std.log.debug("|{s: ^24} | {s: ^18} | {s: ^18} |", .{ "Type", "Base", "Limit" });
    std.log.debug("+-------------------------------------------------------------------+", .{});
    for (boot.payload.records[0..boot.payload.count]) |r| {
        std.log.debug("|{s: ^24} | 0x{x:0>16} | 0x{x:0>16} |", .{ std.enums.tagName(handover.Tags, @enumFromInt(r.tag)).?, r.start, r.start + r.size });
    }
    std.log.debug("+-------------------------------------------------------------------+", .{});
}
