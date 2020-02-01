#pragma once
#include "engine/core/types.h"

namespace custom {

struct Bytecode;

void load_image(Bytecode & bc, u32 asset_id);
void load_imagef(Bytecode & bc, u32 asset_id);
void load_image16(Bytecode & bc, u32 asset_id);

void load_shader(Bytecode & bc, u32 asset_id);

void load_quad(Bytecode & bc, u32 asset_id);
void load_particle_test(Bytecode & bc, u32 asset_id);

}
