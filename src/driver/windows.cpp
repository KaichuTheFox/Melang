#include "moulang/driver.hpp"
#include "moulang/object.hpp"
#include "toolchain.hpp"

#include <Windows.h>
#include <system_error>
#include <vector>

namespace moulang {
namespace {
namespace fs = std::filesystem;

[[noreturn]] void windowsError(const char* action, DWORD error = GetLastError()) {
    throw std::system_error(static_cast<int>(error), std::system_category(), action);
}

class StagingDirectory {
public:
    explicit StagingDirectory(const fs::path& parent) {
        const auto prefix = L".moulang-" + std::to_wstring(GetCurrentProcessId()) + L"-";
        for (unsigned attempt = 0; ; ++attempt) {
            path = parent / (prefix + std::to_wstring(attempt));
            if (fs::create_directory(path)) {
                break;
            }
        }
    }

    StagingDirectory(const StagingDirectory&) = delete;
    StagingDirectory& operator=(const StagingDirectory&) = delete;

    ~StagingDirectory() {
        std::error_code ignored;
        fs::remove_all(path, ignored);
    }

    fs::path path;
};

fs::path compilerDirectory() {
    std::wstring path(512, L'\0');
    for (;;) {
        const DWORD size = GetModuleFileNameW(nullptr, path.data(), static_cast<DWORD>(path.size()));
        if (!size) {
            windowsError("Cannot find moulang.exe");
        }
        if (size < path.size()) {
            path.resize(size);
            return fs::path(path).parent_path();
        }
        path.resize(path.size() * 2);
    }
}

// Quote one argument using the Windows C runtime argument parsing rules.
std::wstring quote(const std::wstring& argument) {
    std::wstring quoted = L"\"";
    std::size_t backslashes = 0;
    for (const wchar_t c : argument) {
        if (c == L'\\') {
            ++backslashes;
            continue;
        }
        quoted.append(c == L'"' ? 2 * backslashes + 1 : backslashes, L'\\');
        backslashes = 0;
        quoted += c;
    }
    quoted.append(2 * backslashes, L'\\');
    quoted += L'"';
    return quoted;
}

DWORD launch(const fs::path& executable, const std::vector<std::wstring>& args, const fs::path& cwd) {
    std::wstring command = quote(executable.native());
    for (const auto& arg : args) {
        command += L" " + quote(arg);
    }
    STARTUPINFOW startup{};
    startup.cb = sizeof(startup);
    PROCESS_INFORMATION process{};
    if (!CreateProcessW(executable.c_str(), command.data(), nullptr, nullptr, TRUE, 0,
            nullptr, cwd.c_str(), &startup, &process)) {
        windowsError("Cannot start process");
    }
    CloseHandle(process.hThread);
    DWORD exitCode = 0;
    if (WaitForSingleObject(process.hProcess, INFINITE) == WAIT_FAILED ||
        !GetExitCodeProcess(process.hProcess, &exitCode)) {
        const auto error = GetLastError();
        CloseHandle(process.hProcess);
        windowsError("Cannot get process exit code", error);
    }
    CloseHandle(process.hProcess);
    return exitCode;
}

fs::path compileAndLink(const Program& program, const fs::path& stage, Optimization optimization) {
    emitObject(program, stage / L"program.obj", optimization);
    // Local ASCII arguments also work when the project directory has a Unicode name.
    fs::copy_file(compilerDirectory() / L"moulang_runtime.lib", stage / L"moulang_runtime.lib");
    const DWORD status = launch(linkerPath, {
        L"/NOLOGO", L"/MACHINE:X64", L"/SUBSYSTEM:CONSOLE", L"/ENTRY:moulang_entry", L"/NODEFAULTLIB",
        L"/OUT:program.exe", L"program.obj", L"moulang_runtime.lib", kernel32Path,
        staticCrtPath, staticCppPath, staticVcruntimePath
    }, stage);
    if (status != 0) {
        throw std::runtime_error("Native linker exited with code " + std::to_string(status) + ".");
    }
    return stage / L"program.exe";
}
}

void buildExecutable(const Program& program, const fs::path& output, Optimization optimization) {
    const auto target = fs::absolute(output);
    const StagingDirectory stage(target.parent_path());
    const auto executable = compileAndLink(program, stage.path, optimization);
    if (!MoveFileExW(executable.c_str(), target.c_str(), MOVEFILE_REPLACE_EXISTING)) {
        windowsError("Cannot publish executable");
    }
}

DWORD runExecutable(const Program& program) {
    const StagingDirectory stage(fs::temp_directory_path());
    const auto executable = compileAndLink(program, stage.path, Optimization::none);
    return launch(executable, {}, fs::current_path());
}
}
