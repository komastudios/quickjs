# quickjs

## Build for Windows
Build:
```
$ zig build --prefix .install/win64 -Dtarget=x86_64-windows-gnu -Doptimize=ReleaseFast
```

Test:
```
$ zig build --prefix .install/win64 -Dtarget=x86_64-windows-gnu -Doptimize=ReleaseFast test
```
