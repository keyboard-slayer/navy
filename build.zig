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

const validLoaders = enum { booboot, limine };

fn fetchResource(b: *std.Build, base: []const u8, root: []const u8) !std.StringHashMap(*std.Build.Module) {
    var resList: std.StringHashMap(*std.Build.Module) = .init(b.allocator);
    errdefer resList.deinit();

    var dir = try std.fs.cwd().openDir(base, .{ .iterate = true });
    defer dir.close();

    var walker = try dir.walk(b.allocator);
    defer walker.deinit();

    while (try walker.next()) |entry| {
        if (std.mem.indexOf(u8, entry.path, root) != null) {
            const path = b.path(b.fmt("{s}/{s}", .{ base, entry.path }));

            var it = std.mem.splitBackwardsScalar(u8, path.dirname().src_path.sub_path, '/');
            const name = it.first();

            try resList.put(name, b.createModule(.{ .root_source_file = path }));
        }
    }

    return resList;
}

fn addImports(resList: std.StringHashMap(*std.Build.Module), module: *std.Build.Module) void {
    var it = resList.keyIterator();
    while (it.next()) |key| {
        const mod = resList.get(key.*);
        module.addImport(key.*, mod.?);
    }
}

fn addRunStep(b: *std.Build, loader: validLoaders, booboot: *std.Build.Step.Compile, kernel: *std.Build.Step.Compile) !void {
    const wf = b.addWriteFiles();

    var configCopy: *std.Build.Step.InstallFile = undefined;
    var efiCopy: *std.Build.Step = undefined;

    switch (loader) {
        .booboot => {
            const config = wf.add("loader.json",
                \\  {
                \\      "entries": [
                \\          {
                \\              "name": "Navy",
                \\              "kernel": "kernel.elf",
                \\              "protocol": "handover"
                \\          }
                \\      ]
                \\  }
            );

            const buildStep = b.addInstallArtifact(booboot, .{});
            configCopy = b.addInstallFile(config, "./sysroot/loader.json");
            efiCopy = &b.addInstallFile(booboot.getEmittedBin(), "./sysroot/efi/boot/bootx64.efi").step;
            efiCopy.dependOn(&buildStep.step);
        },

        .limine => {
            const config = wf.add("limine.conf",
                \\timeout: 0
                \\/navy
                \\  protocol: limine
                \\  kernel_path: boot():/kernel.elf
            );

            configCopy = b.addInstallFile(config, "./sysroot/limine.conf");
            efiCopy = &b.addSystemCommand(&.{
                "curl",
                "-L",
                "-C",
                "-",
                "https://github.com/limine-bootloader/limine/raw/refs/heads/v9.x-binary/BOOTX64.EFI",
                "--create-dirs",
                "-o",
                "./zig-out/sysroot/efi/boot/bootx64.efi",
            }).step;
        },
    }

    const runStep = b.step("run", "Run the project");

    const fetchBios = b.addSystemCommand(&.{
        "curl",
        "-C",
        "-",
        "https://retrage.github.io/edk2-nightly/bin/RELEASEX64_OVMF.fd",
        "-o",
        "./zig-out/bios.fd",
    });

    const kernelCopy = b.addInstallFile(kernel.getEmittedBin(), "./sysroot/kernel.elf");
    const qemuStep = b.addSystemCommand(&.{
        "qemu-system-x86_64",
        "-machine",
        "q35",
        "-no-reboot",
        "-no-shutdown",
        // "-display",
        // "none",
        "-serial",
        "mon:stdio",
        "-drive",
        "format=raw,file=fat:rw:./zig-out/sysroot,media=disk",
        "-bios",
        "./zig-out/bios.fd",
        // "-s",
        // "-S",
    });

    fetchBios.step.dependOn(efiCopy);
    kernelCopy.step.dependOn(&kernel.step);
    qemuStep.step.dependOn(&kernelCopy.step);
    qemuStep.step.dependOn(&fetchBios.step);
    qemuStep.step.dependOn(&configCopy.step);
    runStep.dependOn(&qemuStep.step);
}

pub fn build(b: *std.Build) !void {
    const loader = b.option(validLoaders, "loader", "Bootloader to use (for the run step)") orelse .booboot;
    const arch = b.option(std.Target.Cpu.Arch, "arch", "Target Architecture") orelse .x86_64;
    const archName = std.enums.tagName(std.Target.Cpu.Arch, arch).?;
    const optimize = b.standardOptimizeOption(.{});
    const target: std.Target.Query = .{
        .cpu_arch = arch,
        .os_tag = .freestanding,
        .abi = .none,
        .ofmt = .elf,
    };

    const booboot = b.dependency("booboot", .{}).artifact("booboot");

    const archImpl = b.createModule(.{
        .root_source_file = b.path(b.fmt("src/kernel/archs/{s}/root.zig", .{archName})),
    });

    const kernel = b.addExecutable(.{
        .name = "navy",
        .use_llvm = true,
        .use_lld = true,
        .root_module = b.createModule(.{
            .code_model = .kernel,
            .root_source_file = b.path("src/kernel/main.zig"),
            .target = b.resolveTargetQuery(target),
            .optimize = optimize,
            .imports = &.{
                .{ .name = "arch", .module = archImpl },
            },
        }),
    });

    kernel.linkage = .static;
    kernel.entry = .disabled;
    kernel.want_lto = false;
    kernel.setLinkerScript(b.path(b.fmt("src/linker/kernel-{s}.ld", .{archName})));

    var libs = try fetchResource(b, "src/libs/", "root.zig");
    defer libs.deinit();

    addImports(libs, kernel.root_module);
    addImports(libs, archImpl);

    b.installArtifact(kernel);
    b.installArtifact(booboot);

    try addRunStep(b, loader, booboot, kernel);
}
