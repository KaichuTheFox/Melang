#include "melang/driver.hpp"
#include "melang/parser.hpp"
#include "melang/semantic.hpp"
#include "toolchain.hpp"

#include <Windows.h>
#include <fcntl.h>
#include <io.h>
#include <iostream>
#include <iterator>
#include <sstream>

namespace {
namespace fs = std::filesystem;

void writeDiagnostic(const std::string& text) {
    const auto output = GetStdHandle(STD_ERROR_HANDLE);
    DWORD mode;
    if (GetConsoleMode(output, &mode)) {
        const int bytes = static_cast<int>(text.size());
        const int size = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, text.data(), bytes, nullptr, 0);
        if (size) {
            std::wstring wide(size, L'\0');
            if (MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, text.data(), bytes, wide.data(), size)) {
                DWORD written;
                if (WriteConsoleW(output, wide.data(), size, &written, nullptr)) return;
            }
        }
    }
    std::cerr << text;
}

void help(std::ostream& out) {
    out << "Usage:\n"
           "  melang --version\n"
           "  melang --help\n"
           "  melang run <source.m>\n"
           "  melang check <source.m> [--stdin] [--library]\n"
           "  melang build <source.m> [-o <output.exe>] [-O0|-O2|-O3]\n";
}

bool buildOptions(int argc, wchar_t* argv[], fs::path& output, melang::Optimization& optimization) {
    bool optimizationSet = false;
    for (int i = 3; i < argc; ++i) {
        const std::wstring_view option = argv[i];
        if (option == L"-o" && output.empty() && i + 1 < argc && argv[i + 1][0] != L'\0') {
            output = argv[++i];
        } else if (!optimizationSet && (option == L"-O0" || option == L"-O2" || option == L"-O3")) {
            optimizationSet = true;
            if (option == L"-O2") optimization = melang::Optimization::speed;
            if (option == L"-O3") optimization = melang::Optimization::aggressive;
        } else {
            return false;
        }
    }
    return true;
}

bool checkOptions(int argc, wchar_t* argv[], bool& fromStdin, bool& library) {
    for (int i = 3; i < argc; ++i) {
        const std::wstring_view option = argv[i];
        if (option == L"--stdin" && !fromStdin) fromStdin = true;
        else if (option == L"--library" && !library) library = true;
        else return false;
    }
    return true;
}
}

int wmain(int argc, wchar_t* argv[]) {
    const std::wstring_view command = argc > 1 ? argv[1] : L"";
    if (argc == 2 && command == L"--help") {
        help(std::cout);
        return 0;
    }
    if (argc == 2 && command == L"--version") {
        std::cout << "Melang " << melang::version << '\n';
        return 0;
    }

    const bool run = command == L"run" && argc == 3;
    bool fromStdin = false;
    bool library = false;
    const bool check = command == L"check" && argc >= 3 && checkOptions(argc, argv, fromStdin, library);
    fs::path output;
    auto optimization = melang::Optimization::none;
    const bool build = command == L"build" && argc >= 3 && buildOptions(argc, argv, output, optimization);
    if ((!run && !build && !check) || argv[2][0] == L'\0') {
        help(std::cerr);
        return 2;
    }

    const fs::path input = argv[2];
    std::vector<fs::path> sources;
    try {
        std::optional<std::string> overlay;
        if (fromStdin) {
            if (_setmode(_fileno(stdin), _O_BINARY) == -1) {
                throw std::runtime_error("Cannot open standard input in binary mode.");
            }
            overlay.emplace(std::istreambuf_iterator<char>(std::cin), std::istreambuf_iterator<char>());
        }
        auto program = melang::loadProgram(input, sources,
            overlay ? std::optional<std::string_view>(*overlay) : std::nullopt);
        melang::analyze(program, !library);
        if (check) return 0;
        if (run) {
            return static_cast<int>(melang::runExecutable(program));
        }
        if (output.empty()) output = input.filename().replace_extension(L".exe");
        if (fs::exists(output)) {
            for (const auto& source : sources) {
                if (fs::equivalent(source, output)) {
                    throw std::runtime_error("Output must not overwrite the source file.");
                }
            }
        }
        melang::buildExecutable(program, output, optimization);
        return 0;
    } catch (const melang::Diagnostic& diagnostic) {
        const auto& source = diagnostic.location.file < sources.size() ? sources[diagnostic.location.file] : input;
        std::ostringstream text;
        text << melang::pathText(source) << ':' << diagnostic.location.line << ':'
             << diagnostic.location.column << ": error " << diagnostic.code << ": "
             << diagnostic.what() << '\n';
        writeDiagnostic(text.str());
    } catch (const std::exception& error) {
        std::cerr << "melang: error ML4001: " << error.what() << '\n';
    }
    return 1;
}
