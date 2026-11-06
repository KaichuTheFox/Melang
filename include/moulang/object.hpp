#pragma once

#include "moulang/ast.hpp"

namespace moulang {
enum class Optimization { none, speed, aggressive };

void emitObject(const Program& program, const std::filesystem::path& path, Optimization optimization);
}
