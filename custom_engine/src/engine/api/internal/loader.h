#pragma once
#include "engine/core/types.h"

struct lua_State;

namespace custom {
struct Bytecode;
}

namespace custom {
namespace loader {

void init(Bytecode * bc);

void script(lua_State * L, u32 asset_id);

void image(u32 asset_id);
void imagef(u32 asset_id);
void image16(u32 asset_id);

void uniforms();
void shader(u32 asset_id);

void mesh(u32 asset_id);

}}
