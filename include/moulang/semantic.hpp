#pragma once

#include "moulang/ast.hpp"

namespace moulang {
void analyze(Program& program, bool requireMain = true);
}
