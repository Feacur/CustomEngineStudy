#pragma once
#include "engine/core/types.h"

namespace custom {
struct Bytecode;
}

namespace custom {
namespace runtime {
struct Buffer;
}}

namespace custom {
namespace loader {

void init(Bytecode * bytecode);

void image(u32 asset_id);
void imagef(u32 asset_id);
void image16(u32 asset_id);

void shader(u32 asset_id);

void mesh(u32 asset_id);

u32 create_mesh(u32 local_id, runtime::Buffer const * buffers, u8 count);
u32 create_quad(u32 local_id);
u32 create_cube(u32 local_id);

}}
