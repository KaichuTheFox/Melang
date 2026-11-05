#pragma once

#include "melang/object.hpp"

namespace melang {
void buildExecutable(const Program& program, const std::filesystem::path& output, Optimization optimization);
unsigned long runExecutable(const Program& program);
}
