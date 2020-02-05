#pragma once
#include "engine/core/types.h"

namespace custom {
struct Bytecode;
}

namespace custom {
namespace loader {

void init(Bytecode * bytecode);

void image(u32 asset_id);
void imagef(u32 asset_id);
void image16(u32 asset_id);

void shader(u32 asset_id);

u32 create_quad(u32 local_asset_id, u32 meta_id);
u32 create_particle_test(u32 local_asset_id, u32 meta_id);

}}
