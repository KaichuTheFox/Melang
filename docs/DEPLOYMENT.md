# Windows deployment

## Development compiler

Run cmake --install build/moulang --prefix build/moulang/install. The resulting bin directory contains moulang.exe and moulang_runtime.lib. Add it to PATH to use the compiler from CMD or PowerShell.

The compiler uses the paths recorded at CMake configuration time for lld-link.exe, the x64 Windows SDK kernel32.lib, and the static MSVC libraries libcmt.lib, libcpmt.lib, and libvcruntime.lib. Reconfigure and rebuild if those tools move. If the official LLVM archive refers to the release builder's DIA SDK, CMake looks for the x64 diaguids.lib in the current Visual Studio installation.

## Generated executables

Moulang links the runtime statically into the generated Windows x64 executable. The Hello World executable imports KERNEL32.dll.

## VS Code extension

The VSIX installs separately from the native compiler. See the [README](../README.md#visual-studio-code) for packaging and setup. The package identity is `KaichuTheFox.moulang`. Earlier packages can be retained as release archives.

A standalone Windows installer and a relocatable compiler distribution are future work.
