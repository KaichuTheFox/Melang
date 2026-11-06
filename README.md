# Moulang

<img src="vscode-extension/images/moulang-icon.png" alt="Moulang logo" width="128">

Moulang is a statically typed programming language from the HAZOS development team. The project adopted the Moulang name while keeping its `.m` source format. The compiler is written in C++20 and uses LLVM to produce native Windows x64 executables. The project aims to make native compilation and source-level diagnostics easy to use while keeping the language small.

## Hello, world

Save this as `hello.m`.

```moulang
main() {
    print("Hello Moulang!")
}
```

## Build and install the compiler

Moulang currently targets Windows x64. Install Visual Studio with the **Desktop development with C++** workload, the Windows SDK, CMake 3.25 or later, Ninja, and an LLVM 22 or later development package with the X86 target. Extract the official Windows x64 MSVC LLVM archive. The archive should contain lib/cmake/llvm/LLVMConfig.cmake and bin/lld-link.exe. This project was built with LLVM 22.1.8.

From an x64 **Developer PowerShell for VS**, open the repository root. Adjust the LLVM path to match the location where you extracted it.

```powershell
$llvm = (Resolve-Path '.toolchain/clang+llvm-22.1.8-x86_64-pc-windows-msvc').Path
cmake -S . -B build/moulang -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_TRY_COMPILE_CONFIGURATION=Release "-DLLVM_DIR=$llvm/lib/cmake/llvm" "-DCMAKE_LINKER=$llvm/bin/lld-link.exe"
cmake --build build/moulang
ctest --test-dir build/moulang --output-on-failure
cmake --install build/moulang --prefix build/moulang/install
```

The compiler and its runtime library are in `build/moulang/bin`. Installation copies them into `build/moulang/install/bin`. Keep `moulang.exe` and `moulang_runtime.lib` together. Add that `bin` directory to `PATH` to use the commands below.

## Run, build, and check

```powershell
moulang --version
moulang --help
moulang run examples/hello.m
moulang build examples/hello.m -o build/moulang/hello.exe
.\build\moulang\hello.exe
moulang check examples/objects.m
```

- `run` compiles and runs a temporary executable. It forwards the program's exit code.
- `build` writes an executable. Without `-o`, it writes the executable to the current directory using the source file's base name. Use `-O0`, `-O2`, or `-O3` to set optimization.
- `check` parses and type checks source. It accepts `--stdin` for unsaved text and `--library` for source that does not need a `main` entry point.

## Language example

```moulang
int doubled(int value) {
    return value * 2
}

main() {
    int numbers[] = [10, 20, 30]
    foreach (item in numbers) {
        print(doubled(item))
    }
}
```

Moulang also supports value structs, nullable values, and relative imports. Classes, fixed integer arrays, and local view checks have limited implementations. See the [language specification](docs/LANGUAGE_SPEC.md) and [implementation status](docs/IMPLEMENTATION_STATUS.md) for details.

## Visual Studio Code

Package the extension from vscode-extension with the VS Code Extension Manager CLI:

```powershell
Push-Location vscode-extension
vsce package --allow-missing-repository --out ../build/moulang/moulang-0.1.3.vsix
Pop-Location
```

In VS Code, select **Extensions > Install from VSIX** and open that package. Open the folder containing your `.m` file. If the compiler is outside `PATH`, set **Moulang: Executable Path** (`moulang.executablePath`) to the full path to `moulang.exe`. If you configured an earlier extension, copy its compiler path into this setting and disable the earlier extension to avoid duplicate file associations.

Open a saved `.m` file and click the play button in the upper right of the editor. The **Moulang: Run Current File** command in the Command Palette does the same thing. The extension saves edits before running. Output appears in the terminal and compiler errors appear in **Problems**. **Moulang: Live Diagnostics** checks unsaved changes in a trusted workspace.

To load the extension from source, run this from the repository root:

```powershell
code --extensionDevelopmentPath="$((Resolve-Path vscode-extension).Path)" .
```

## Troubleshooting

- **moulang is not recognized.** Add the compiler directory to PATH and reopen PowerShell, or use the full executable path.
- **CMake cannot find LLVM.** Point LLVM_DIR to the extracted package's lib/cmake/llvm directory. Use an x64 Developer PowerShell and point CMAKE_LINKER to lld-link.exe from the same archive.
- **Ninja misses header changes with localized MSVC.** Set MOULANG_MSVC_INCLUDE_PREFIX to the literal prefix printed before file paths by cl /showIncludes. Then rebuild with cmake --build build/moulang --clean-first.
- **A Debug build reports MSVC error C1902.** Configure a fresh Debug build directory with `-DCMAKE_MSVC_DEBUG_INFORMATION_FORMAT=Embedded`. For Debug builds with CMake older than 4.0, also pass `"-DCMAKE_CXX_FLAGS_DEBUG=/Od /Ob0"` to keep MSVC runtime check instrumentation out of the standalone runtime.
- **The compiler cannot find the linker, runtime, or an SDK library.** Keep moulang_runtime.lib next to the compiler. Reconfigure and rebuild if you moved the LLVM or Visual Studio installation.

## Project information

Moulang is developed by HAZOS and distributed under the [MIT License](LICENSE).

- [Compiler architecture](docs/COMPILER_ARCHITECTURE.md)
- [User guide](docs/USER_GUIDE.md)
- [Changelog](CHANGELOG.md)
- [Development roadmap](docs/ROADMAP.md)
- [Windows deployment](docs/DEPLOYMENT.md)
