const std = @import("std");
const fs = std.fs;

const quickjs_sources = [_][]const u8{
    "cutils.c",
    "libregexp.c",
    "libunicode.c",
    "quickjs.c",
    "quickjs-libc.c",
    "quickjs-extern.c",
};

const quickjs_cflags: []const []const u8 = &.{
    "-std=c99",
    "-DQUICKJS_BUILD",
};

const quickjs_cflags_android: []const []const u8 = quickjs_cflags ++ [_][]const u8{
    "-fno-builtin-bcmp",
};

const quickjs_cflags_ios: []const []const u8 = quickjs_cflags ++ [_][]const u8{
    //"-fno-builtin-bcmp",
};

const quickjs_public_headers = [_][]const u8{ "quickjs.h", "quickjs-libc.h" };

pub fn build(b: *std.Build) !void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    const android = b.option(bool, "android", "Target Android") orelse false;

    const lib_static = b.addStaticLibrary(.{
        .name = "quickjs-static",
        .target = target,
        .optimize = optimize,
    });
    lib_static.addIncludePath(".");

    const build_shared_lib = lib_static.target_info.target.os.tag != .ios;

    switch (lib_static.target_info.target.os.tag) {
        .windows => {
            lib_static.defineCMacro("QUICKJS_DLL", null);
            lib_static.defineCMacro("JS_STRICT_NAN_BOXING", null);
        },
        else => {
            lib_static.defineCMacro("_GNU_SOURCE", null);
        },
    }
    if (android) {
        lib_static.defineCMacro("__ANDROID__", null);
        lib_static.addCSourceFiles(&quickjs_sources, quickjs_cflags_android);
    } else if (lib_static.target_info.target.os.tag == .ios) {
        if (b.sysroot) |sysroot| {
            lib_static.addIncludePath(try fs.path.join(b.allocator, &.{ sysroot, "usr/include" }));
        } else {
            std.log.warn("Missing sysroot", .{});
        }

        lib_static.addCSourceFiles(&quickjs_sources, quickjs_cflags_ios);
    } else {
        lib_static.addCSourceFiles(&quickjs_sources, quickjs_cflags);
    }
    lib_static.linkLibC();

    b.installArtifact(lib_static);
    for (quickjs_public_headers) |h| lib_static.installHeader(h, h);

    if (build_shared_lib) {
        const lib_shared = b.addSharedLibrary(.{
            .name = "quickjs",
            .target = target,
            .optimize = optimize,
        });
        lib_shared.addIncludePath(".");

        switch (lib_shared.target_info.target.os.tag) {
            .windows => {
                lib_shared.defineCMacro("QUICKJS_DLL", null);
                lib_shared.defineCMacro("JS_STRICT_NAN_BOXING", null);
            },
            else => {
                lib_shared.defineCMacro("_GNU_SOURCE", null);
            },
        }
        if (android) {
            lib_shared.defineCMacro("__ANDROID__", null);
            lib_shared.addCSourceFiles(&quickjs_sources, quickjs_cflags_android);
        } else {
            lib_shared.addCSourceFiles(&quickjs_sources, quickjs_cflags);
        }
        lib_shared.linkLibC();

        b.installArtifact(lib_shared);

        const test_extern = b.addExecutable(.{
            .name = "test-extern",
            .target = target,
            .optimize = optimize,
        });
        test_extern.addCSourceFiles(&.{"test-extern.c"}, &.{});
        test_extern.linkLibrary(lib_shared);
        test_extern.defineCMacro("QUICKJS_DLL", null);
        b.installArtifact(test_extern);

        const run_cmd = b.addRunArtifact(test_extern);
        run_cmd.step.dependOn(b.getInstallStep());
        if (b.args) |args| {
            run_cmd.addArgs(args);
        }
        const run_step = b.step("test", "Run tests");
        run_step.dependOn(&run_cmd.step);
    }
}
