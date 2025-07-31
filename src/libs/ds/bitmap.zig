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

pub const Bitmap = struct {
    buf: [*]u8,
    len: usize,

    pub fn from_mem(mem: [*]u8, size: usize) Bitmap {
        return .{ .buf = mem, .len = size };
    }

    pub fn set(self: *Bitmap, bit: usize) void {
        self.buf[bit / 8] |= @as(u8, 1) << @truncate(bit % 8);
    }

    pub fn set_range(self: *Bitmap, start: usize, len: usize) void {
        for (start..(start + len)) |i| {
            self.set(i);
        }
    }

    pub fn is_set(self: *Bitmap, bit: usize) bool {
        return ((self.buf[bit / 8] >> @truncate(bit % 8)) & 1) == 1;
    }

    pub fn unset(self: *Bitmap, bit: usize) void {
        self.buf[bit / 8] &= ~(@as(u8, 1) << @truncate(bit % 8));
    }

    pub fn unset_range(self: *Bitmap, start: usize, len: usize) void {
        for (start..(start + len)) |i| {
            self.unset(i);
        }
    }

    pub fn fill(self: *Bitmap, value: u8) void {
        for (0..self.len) |i| {
            self.buf[i] = value;
        }
    }
};
