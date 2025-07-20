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

const as = @import("./asm.zig");

pub fn setupSSE() void {
    as.cr0.clear(as.Cr0Field.x87FpuEmulation);
    as.cr0.set(as.Cr0Field.monitorCoprocessor);

    as.cr4.set(as.Cr4Field.osfxsr);
    as.cr4.set(as.Cr4Field.osxmmexcpt);
}
