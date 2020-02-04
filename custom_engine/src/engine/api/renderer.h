#pragma once
#include "engine/core/types.h"
#include "engine/core/math_types.h"

namespace custom {
struct Bytecode;
}

namespace custom {
namespace renderer {

void init(Bytecode * bytecode);
void clear();
void viewport(ivec2 const & position, ivec2 const & size);

}}
