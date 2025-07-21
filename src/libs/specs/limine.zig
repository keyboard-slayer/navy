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

fn craftRequest(n1: u64, n2: u64) [4]u64 {
    return .{ 0xc7b1dd30df4c8b88, 0x0a82e883a194f07b, n1, n2 };
}

pub const BaseRevision = extern struct {
    magic: [2]u64 = .{ 0xf9562b2d5c95a6c8, 0x6a7b384944536bdc },
    revision: u64 = 3,

    pub fn isValid(self: BaseRevision) bool {
        return self.magic[1] != 0x6a7b384944536bdc;
    }

    pub fn isSupported(self: BaseRevision) bool {
        return self.revision == 0;
    }
};

pub const ExecutableAddress = extern struct {
    id: [4]u64 = craftRequest(0x71ba76863cc55f63, 0xb2644a48c516a487),
    revision: u64 = 0,
    response: ?*extern struct { revision: u64, physAddr: u64, virtAddr: u64 } = null,
};

pub const Rsdp = extern struct {
    id: [4]u64 = craftRequest(0xc5e77b6b397e7b43, 0x27637845accdcf3c),
    revision: u64 = 0,
    response: ?*extern struct { revision: u64, address: u64 } = null,
};

pub const MmapType = enum(u64) {
    usable = 0,
    reserved = 1,
    acpi_reclaimable = 2,
    acpi_nvs = 3,
    bad_memory = 4,
    bootloader_reclaimable = 5,
    executable_and_modules = 6,
    framebuffer = 7,
};

pub const Mmap = extern struct {
    id: [4]u64 = craftRequest(0x67cf3d9d378a806f, 0xe304acdfc50c3c62),
    revision: u64 = 0,
    response: ?*struct { revision: u64, count: u64, entries: ?[*]*extern struct { start: u64, size: u64, type: MmapType } } = null,
};

pub const Uuid = extern struct {
    a: u32,
    b: u16,
    c: u16,
    d: [8]u8,
};

pub const Module = extern struct {
    id: [4]u64 = craftRequest(0x3e7e279702be32af, 0xca1c4f3bd1280cee),
    revision: u64 = 0,
    response: ?*struct {
        revision: u64,
        count: u64,
        entries: ?[*]extern struct {
            revision: u64,
            start: u64,
            size: u64,
            path: [*:0]u8,
            string: [*:0]u8,
            media_type: u32,
            unused: u32,
            tftp_ip: u32,
            tftp_port: u32,
            partition_index: u32,
            mbr_disk_id: u32,
            gpt_disk_uuid: Uuid,
            gpt_part_uuid: Uuid,
            part_uuid: Uuid,
        },
    } = null,
};
