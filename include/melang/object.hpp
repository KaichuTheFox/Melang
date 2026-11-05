#pragma once

#include "melang/ast.hpp"

namespace melang {
enum class Optimization { none, speed, aggressive };

void emitObject(const Program& program, const std::filesystem::path& path, Optimization optimization);
}
