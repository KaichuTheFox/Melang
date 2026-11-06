# Implementation status

## Available

- Native Windows x64 compilation from UTF-8 .m source through LLVM IR and COFF to PE executables. The CLI provides --help, --version, run, build, and check.
- Local variables, type inference with var, arithmetic and comparison, short-circuit logic, and explicit numeric conversions. Numeric types include char, short, int, long, float, and double.
- Functions, explicit and inferred return types, conditionals, loops, break, and continue.
- Integer slices, append, checked indexing, zero-copy read-only views, and foreach.
- Value structs, positional initialization, nested fields, and field assignment.
- Nullable values, coalescing, safe member access, and limited flow analysis.
- Relative imports, file-level public and private visibility, UTF-8 file I/O, and LLVM optimization levels -O0, -O2, and -O3.
- VS Code highlighting, completion, snippets, execution, compiler diagnostics, and opt-in diagnostics for unsaved edits.

## Limited support

- Local fixed integer arrays support value copies and local view restrictions.
- Classes support reference semantics, constructors, access control, nullable calls, and limited single inheritance.
- Class objects and heap-backed slice buffers live until process exit.

## Future work

- Arrays and slices of other types, general lifetime and ownership analysis, method overriding, and module aliases.
- Garbage collection, exceptions, multiple inheritance, generics, unions, pointers, unmanaged memory, and concurrency.
- A language server, source-level debugger, Windows installer, and a larger standard library.

See the [compiler integration suite](../tests/integration/compiler.ps1) and the [VS Code host suite](../tests/vscode/suite.js) for the scenarios covered by automated tests.
