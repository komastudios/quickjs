const std = @import("std");

const quickjs_sources = [_][]const u8{
    "cutils.c",
    "libregexp.c",
    "libunicode.c",
    "quickjs.c",
    "quickjs-libc.c",
    "quickjs-extern.c",
};

const quickjs_cflags: []const []const u8 = &.{ "-std=c99", "-DQUICKJS_BUILD" };

const quickjs_public_headers = [_][]const u8{ "quickjs.h", "quickjs-libc.h" };

pub fn build(b: *std.Build) !void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    const lib = b.addSharedLibrary(.{
        .name = "quickjs",
        .target = target,
        .optimize = optimize,
    });
    lib.addIncludePath(".");

    const t = lib.target_info.target;
    switch (t.os.tag) {
        .windows => {
            lib.defineCMacro("QUICKJS_DLL", null);
            lib.defineCMacro("JS_STRICT_NAN_BOXING", null);
        },
        else => {
            lib.defineCMacro("_GNU_SOURCE", null);
        },
    }
    lib.defineCMacro("CONFIG_VERSION", "\"2021-03-27\""); // TODO: read from version file
    lib.addCSourceFiles(&quickjs_sources, quickjs_cflags);
    lib.linkLibC();

    b.installArtifact(lib);
    for (quickjs_public_headers) |h| lib.installHeader(h, h);

    const test_extern = b.addExecutable(.{
        .name = "test-extern",
        .target = target,
        .optimize = optimize,
    });
    test_extern.addCSourceFiles(&.{"test-extern.c"}, &.{});
    test_extern.linkLibrary(lib);
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
