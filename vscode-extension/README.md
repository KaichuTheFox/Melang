# Moulang for VS Code

Language support and native execution for Moulang (.m), the programming language from the HAZOS development team.

The extension recognizes .m files and provides syntax highlighting, snippets, completion, and compiler diagnostics. It uses a separate Windows x64 Moulang compiler.

1. Open the folder containing your .m file.
2. Set **Moulang: Executable Path** to the full path to moulang.exe if it is outside PATH.
3. Open a saved `.m` file and click the play button in the upper right of the editor, or run **Moulang: Run Current File** from the Command Palette. Output appears in the terminal and source diagnostics appear in **Problems**.

Enable **Moulang: Live Diagnostics** in a trusted workspace to check unsaved edits. This package uses the `KaichuTheFox.moulang` identity and `moulang.executablePath` setting. If you installed an earlier extension, set the compiler path again and disable the earlier extension to avoid duplicate file associations.

The extension is licensed under MIT.
