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

const bootPayload = @import("arch").boot.bootPayload;

const Bitmap = @import("ds").Bitmap;

var bitmap: ?Bitmap = null;
var freePages: usize = 0;
const log = std.log.scoped(.pmm);

pub fn lower2upper(lower: u64) usize {
    return handover.UPPER_HALF + lower;
}

pub fn upper2lower(upper: u64) usize {
    return upper - handover.UPPER_HALF;
}

pub fn availableMem() usize {
    return freePages * std.heap.pageSize();
}

pub fn setup() !void {
    var bitmapSize: usize = undefined;
    var index = bootPayload().count - 1;

    while (index > 0) : (index -= 1) {
        const entry = &bootPayload().records[index];
        if (@as(handover.Tags, @enumFromInt(entry.tag)) != .END) {
            bitmapSize = std.mem.alignForward(usize, entry.size / std.heap.pageSize() / 8, std.heap.pageSize());
            break;
        }
    }

    log.debug("Bitmap size: {}", .{bitmapSize});

    for (0..bootPayload().count) |i| {
        const entry = &bootPayload().records[i];
        if (@as(handover.Tags, @enumFromInt(entry.tag)) == .FREE and entry.size >= bitmapSize) {
            const start: usize = if (entry.start == 0) std.heap.pageSize() else entry.start;

            log.debug("Bitmap base address: 0x{x:0>16}", .{start});
            bitmap = Bitmap.from_mem(@ptrFromInt(lower2upper(start)), bitmapSize);
            entry.size -= bitmapSize;
            entry.start += bitmapSize;
            break;
        }
    }

    if (bitmap == null) {
        return error.BitmapNull;
    }

    bitmap.?.fill(0xff);

    for (0..bootPayload().count) |i| {
        const entry = bootPayload().records[i];
        if (@as(handover.Tags, @enumFromInt(entry.tag)) == .FREE) {
            const start = std.mem.alignBackward(
                usize,
                entry.start,
                std.heap.pageSize(),
            ) / std.heap.pageSize();

            const len = std.mem.alignForward(
                usize,
                entry.size,
                std.heap.pageSize(),
            ) / std.heap.pageSize();

            freePages += len;
            bitmap.?.unset_range(start, len);
        }
    }

    bitmap.?.set(0);

    log.debug("Free pages: {}", .{freePages});
    log.debug("Pmm initialized", .{});
}

// === Allocator =======================================================

var try_again: bool = false;
var last_page: usize = 0;

pub fn alloc(ctx: *anyopaque, len: usize, ptr_align: std.mem.Alignment, ret_addr: usize) ?[*]u8 {
    if (bitmap == null) {
        return null;
    }

    const npage = std.mem.alignForward(usize, len, std.heap.pageSize()) / std.heap.pageSize();
    var page_available: usize = 0;
    var base: usize = last_page;

    while (page_available < npage) {
        if (!bitmap.?.is_set(last_page)) {
            page_available += 1;
        } else {
            base = last_page + 1;
            page_available = 0;
        }
        last_page += 1;
    }

    if (page_available == npage) {
        bitmap.?.set_range(base, npage);
        return @as([*]align(std.heap.pageSize()) u8, @ptrFromInt(lower2upper(base * std.heap.pageSize())))[0..len].ptr;
    } else if (!try_again) {
        log.debug("Couldn't allocate {} pages, trying again ...", .{npage});
        try_again = true;
        last_page = 0;
        return alloc(ctx, len, ptr_align, ret_addr);
    } else {
        @panic("Couldn't allocate physical memory, out of memory");
    }
}

pub fn free(_: *anyopaque, buf: []u8, _: std.mem.Alignment, _: usize) void {
    if (bitmap == null) {
        return;
    }
    const n_pages = std.mem.alignForward(
        usize,
        buf.len,
        std.heap.pageSize(),
    ) / std.heap.pageSize();

    const base = upper2lower(@intFromPtr(buf.ptr)) / std.heap.pageSize();
    bitmap.?.unset_range(base, n_pages);
}

pub const PageAllocator = struct {
    pub fn new() PageAllocator {
        return .{};
    }

    pub fn allocator(self: *PageAllocator) std.mem.Allocator {
        return .{
            .ptr = self,
            .vtable = &.{
                .alloc = alloc,
                .free = free,
                .remap = std.mem.Allocator.noRemap,
                .resize = std.mem.Allocator.noResize,
            },
        };
    }
};
