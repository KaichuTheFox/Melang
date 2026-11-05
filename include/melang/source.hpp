#pragma once

#include <cstddef>
#include <filesystem>
#include <stdexcept>
#include <string>

namespace melang {
struct Location {
    std::size_t line = 1;
    std::size_t column = 1;
    std::size_t file = 0;
};

class Diagnostic : public std::runtime_error {
public:
    Diagnostic(Location location, const char* code, const char* message)
        : std::runtime_error(message), location(location), code(code) {}

    Location location;
    const char* code;
};

std::string pathText(const std::filesystem::path& path);
std::string readSource(const std::filesystem::path& path);
void validateSource(std::string& source);
}
