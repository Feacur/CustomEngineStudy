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

namespace custom {
namespace runtime {
struct Buffer;
}}

namespace custom {
namespace loader {

u32 create_mesh(Bytecode * bc, u32 local_id, runtime::Buffer const * buffers, u8 count);
u32 create_quad_xy(Bytecode * bc, u32 local_id);
u32 create_quad_xz(Bytecode * bc, u32 local_id);
u32 create_quad2d(Bytecode * bc, u32 local_id);
u32 create_cube(Bytecode * bc, u32 local_id);

}}
