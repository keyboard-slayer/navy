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

pub fn in8(port: u16) u8 {
    return asm volatile ("inb %[port],%[ret]"
        : [ret] "={al}" (-> u8),
        : [port] "N{dx}" (port),
    );
}

pub fn out8(port: u16, value: u8) void {
    asm volatile ("outb %[value],%[port]"
        :
        : [value] "{al}" (value),
          [port] "N{dx}" (port),
    );
}

const Cr = struct {
    number: u8,

    pub fn read(self: *Cr) u64 {
        var ret: u64 = 0;

        switch (self.number) {
            0 => asm volatile ("mov %%cr0, %[ret]"
                : [ret] "={rax}" (ret),
                :
                : .{ .rax = true }
            ),
            2 => asm volatile ("mov %%cr2, %[ret]"
                : [ret] "={rax}" (ret),
                :
                : .{ .rax = true }
            ),
            3 => asm volatile ("mov %%cr3, %[ret]"
                : [ret] "={rax}" (ret),
                :
                : .{ .rax = true }
            ),
            4 => asm volatile ("mov %%cr4, %[ret]"
                : [ret] "={rax}" (ret),
                :
                : .{ .rax = true }
            ),
            else => unreachable,
        }

        return ret;
    }

    pub fn write(self: *Cr, value: u64) void {
        switch (self.number) {
            0 => asm volatile ("mov %[value], %%cr0"
                :
                : [value] "r" (value),
                : .{ .memory = true }
            ),
            2 => asm volatile ("mov %[value], %%cr2"
                :
                : [value] "r" (value),
                : .{ .memory = true }
            ),
            3 => asm volatile ("mov %[value], %%cr3"
                :
                : [value] "r" (value),
                : .{ .memory = true }
            ),
            4 => asm volatile ("mov %[value], %%cr4"
                :
                : [value] "r" (value),
                : .{ .memory = true }
            ),
            else => unreachable,
        }
    }

    pub fn clear(self: *Cr, value: u64) void {
        const val = self.read();
        self.write(val & ~(value));
    }

    pub fn set(self: *Cr, value: u64) void {
        const val = self.read();
        self.write(val | value);
    }
};

pub var cr0 = Cr{ .number = 0 };
pub var cr2 = Cr{ .number = 2 };
pub var cr3 = Cr{ .number = 3 };
pub var cr4 = Cr{ .number = 4 };

pub const Cr0Field = struct {
    pub const protectedMode = 1 << 0;
    pub const monitorCoprocessor = 1 << 1;
    pub const x87FpuEmulation = 1 << 2;
    pub const taskSwitched = 1 << 3;
    pub const extensionType = 1 << 4;
    pub const numericError = 1 << 5;
    pub const writeProtect = 1 << 16;
    pub const alignmentMask = 1 << 18;
    pub const notWriteThrough = 1 << 29;
    pub const cacheDisable = 1 << 30;
    pub const paging = 1 << 31;
};

pub const Cr4Field = struct {
    pub const virtualMode = 1 << 0;
    pub const protectedModeVirtualInterrupts = 1 << 1;
    pub const timeStampDisable = 1 << 2;
    pub const debuggingExtensions = 1 << 3;
    pub const pageSizeExtensions = 1 << 4;
    pub const physicalAddrESSEXTENSION = 1 << 5;
    pub const machineCheck = 1 << 6;
    pub const pageGlobal = 1 << 7;
    pub const performanceMonitoringCounter = 1 << 8;
    pub const osfxsr = 1 << 9;
    pub const osxmmexcpt = 1 << 10;
    pub const userModeInstructionPrevention = 1 << 11;
    pub const virtualMachineExtensions = 1 << 13;
    pub const saferModeExtensions = 1 << 14;
    pub const fgsbase= 1 << 16;
    pub const pcide = 1 << 17;
    pub const osxsave = 1 << 18;
    pub const smep = 1 << 20;
    pub const smap = 1 << 21;
    pub const protectionKey = 1 << 22;
    pub const controlFlowEnforcement = 1 << 23;
    pub const protectionKeySupervisorPage = 1 << 24;
};

pub const Msr = struct {
    pub const apic = 0x1B;
    pub const efer = 0xC0000080;
    pub const star = 0xC0000081;
    pub const lstar = 0xC0000082;
    pub const compatStar = 0xC0000083;
    pub const syscall_flag_mask = 0xC0000084;
    pub const fs_base = 0xC0000100;
    pub const gs_base = 0xC0000101;
    pub const kern_gs_base = 0xc0000102;
};

pub fn writeMsr(msr: u64, value: u64) void {
    const low: u32 = @truncate(value);
    const high: u32 = @truncate(value >> 32);

    asm volatile ("wrmsr"
        :
        : [_] "{rcx}" (msr),
          [_] "{eax}" (low),
          [_] "{edx}" (high),
    );
}

pub fn readMsr(msr: u64) u64 {
    var low: u32 = 0;
    var high: u32 = 0;

    asm volatile ("rdmsr"
        : [low] "={eax}" (low),
          [high] "={edx}" (high),
        : [msr] "{rcx}" (msr),
    );

    return @as(u64, @intCast(high)) << 32 | low;
}

pub fn readVolatile(T: type, addr: usize) T {
    return @as(*align(1) volatile T, @ptrFromInt(addr)).*;
}

pub fn writeVolatile(T: type, addr: usize, value: T) void {
    @as(*align(1) volatile T, @ptrFromInt(addr)).* = value;
}

pub fn pause() void {
    asm volatile ("pause");
}

pub fn hlt() noreturn {
    while (true) {
        asm volatile ("hlt");
    }
}

pub fn disableInterrupts() void {
    asm volatile ("cli");
}

pub fn enableInterrupts() void {
    asm volatile ("sti");
}
