# Melang

Melang is an independent programming language that compiles `.m` source files into native Windows x64 executables. It is implemented in C++20 with LLVM and supports functions, control flow, value-type structs, integer slices, and basic nullable types.

```melang
main() {
    print("Hello Melang!")
}
```

## Build the compiler

Requirements: the **Desktop development with C++** workload for Visual Studio, the Windows SDK, CMake 3.25 or later, Ninja, and the LLVM development package. Extract the official LLVM Windows x64 MSVC archive and verify that `lib/cmake/llvm/LLVMConfig.cmake` exists. This project was built with LLVM 22.1.8. [LLVM 22.1.8 release](https://github.com/llvm/llvm-project/releases/tag/llvmorg-22.1.8)

Open the project folder in an x64 **Developer PowerShell for VS** environment. Set `$llvm` to the actual path where you extracted LLVM.

```powershell
$llvm = (Resolve-Path '.toolchain/clang+llvm-22.1.8-x86_64-pc-windows-msvc').Path
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_TRY_COMPILE_CONFIGURATION=Release "-DLLVM_DIR=$llvm/lib/cmake/llvm" "-DCMAKE_LINKER=$llvm/bin/lld-link.exe"
cmake --build build
ctest --test-dir build --output-on-failure
```

The compiler build and Melang program linking use `lld-link.exe` from the official LLVM archive.

## Run Melang programs

```powershell
.\build\bin\melang.exe --version
.\build\bin\melang.exe --help
.\build\bin\melang.exe run examples/hello.m
.\build\bin\melang.exe build examples/hello.m -o build/hello.exe
.\build\hello.exe
.\build\bin\melang.exe run examples/variables.m
.\build\bin\melang.exe run examples/control.m
.\build\bin\melang.exe run examples/slices-and-null.m
.\build\bin\melang.exe run examples/numbers.m
.\build\bin\melang.exe run examples/structs.m
.\build\bin\melang.exe run examples/advanced.m
.\build\bin\melang.exe run examples/objects.m
.\build\bin\melang.exe check examples/objects.m
.\build\bin\melang.exe build examples/advanced.m -O3 -o build/advanced.exe
```

Both `hello` commands print `Hello Melang!`. `variables.m` demonstrates type inference, operator precedence, and reassignment. If `-o` is omitted, the compiler creates an `.exe` with the same base name as the source file in the current directory. Add `build/bin` to PATH to invoke the compiler as `melang` from CMD or PowerShell.

## Visual Studio Code

The local extension package is `build/melang-0.1.0.vsix`. In VS Code, open the Extensions view and select **Install from VSIX…**, then open a Melang project folder. See the [VS Code extension installation guide](https://code.visualstudio.com/docs/configure/extensions/extension-marketplace).

To work on the extension source, use the Extension Development Host:

1. From the project root, run `code --extensionDevelopmentPath="$((Resolve-Path vscode-extension).Path)" .`.
2. In the new Extension Development Host window, open `examples/hello.m`.
3. If the compiler is not on PATH, set **Melang: Executable Path** to the absolute path of `build/bin/melang.exe`.
4. Run **Melang: Run Current File** from the Command Palette. Output appears in the integrated terminal. Compiler errors with source locations appear in the **Problems** panel.

Enable **Melang: Live Diagnostics** in a trusted workspace to check edits before saving. Diagnostics do not create an executable or run the program.

In a Melang file, type `p` and select `print` from the suggestions to insert `print()` with the cursor inside the parentheses. Press `Ctrl+Space` to open suggestions manually.

## Documentation

- [Language specification](docs/LANGUAGE_SPEC.md)
- [Implementation status](docs/IMPLEMENTATION_STATUS.md)
- [Compiler architecture](docs/COMPILER_ARCHITECTURE.md)
- [Development roadmap](docs/ROADMAP.md)
- [Windows deployment](docs/DEPLOYMENT.md)
