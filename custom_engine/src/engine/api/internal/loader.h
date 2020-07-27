#pragma once
#include "engine/core/types.h"

struct lua_State;

namespace custom {
struct Bytecode;
}

namespace custom {
namespace loader {

void script(lua_State * L, u32 asset_id);

void image(Bytecode * bc, u32 asset_id);
void imagef(Bytecode * bc, u32 asset_id);
void image16(Bytecode * bc, u32 asset_id);

void uniforms(Bytecode * bc);
void shader(Bytecode * bc, u32 asset_id);

void mesh_obj(Bytecode * bc, u32 asset_id);

}}
