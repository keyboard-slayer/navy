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
const kib = @import("utils").mem.kib;
const limine = @import("specs").limine;
const handover = @import("handover");

export var revision: limine.BaseRevision linksection(".limine_requests") = .{};
export var exeAddr: limine.ExecutableAddress linksection(".limine_requests") = .{};
export var rsdpAddr: limine.Rsdp linksection(".limine_requests") = .{};
export var mmap: limine.Mmap linksection(".limine_requests") = .{};
export var modules: limine.Module linksection(".limine_request") = .{};

var handoverBuffer: [kib(16)]u8 linksection(".bss") = undefined;

pub fn apply() !*handover.Payload {
    if (!revision.isSupported() or !revision.isValid()) {
        return error.invalidLimineBaseRevision;
    }

    var builder: handover.Builder = try .init(&handoverBuffer);

    try builder.append(.{
        .tag = @intFromEnum(handover.Tags.MAGIC),
    });

    try builder.append(.{
        .tag = @intFromEnum(handover.Tags.SELF),
        .start = @intFromPtr(&handoverBuffer),
        .size = kib(16),
    });

    if (exeAddr.response) |addr| {
        try builder.append(.{
            .tag = @intFromEnum(handover.Tags.KERNEL),
            .start = addr.physAddr,
        });
    } else {
        return error.cantGetKernelAddressInfo;
    }

    if (rsdpAddr.response) |addr| {
        try builder.append(.{
            .tag = @intFromEnum(handover.Tags.RSDP),
            .start = addr.address,
        });
    } else {
        return error.cantGetRsdp;
    }

    if (mmap.response) |m| {
        for (0..m.count) |i| {
            const tag: handover.Tags = switch (m.entries.?[i].type) {
                .usable => handover.Tags.FREE,
                .reserved, .acpi_nvs, .bad_memory, .executable_and_modules, .framebuffer => handover.Tags.RESERVED,
                .acpi_reclaimable, .bootloader_reclaimable => handover.Tags.LOADER,
            };

            try builder.append(.{
                .tag = @intCast(@intFromEnum(tag)),
                .start = m.entries.?[i].start,
                .size = m.entries.?[i].size,
            });
        }
    }

    if (modules.response) |m| {
        for (0..m.count) |i| {
            const entry = m.entries.?[i];
            const name = entry.path[0..std.mem.len(entry.path)];

            try builder.append(.{
                .tag = @intFromEnum(handover.Tags.FILE),
                .start = entry.start,
                .size = entry.size,
                .content = .{ .file = .{ .name = builder.addString(name) } },
            });
        }
    }

    return @ptrFromInt(builder.finalize("limine", 0));
}
