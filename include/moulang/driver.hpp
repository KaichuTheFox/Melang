#pragma once

#include "moulang/object.hpp"

namespace moulang {
void buildExecutable(const Program& program, const std::filesystem::path& output, Optimization optimization);
unsigned long runExecutable(const Program& program);
}
