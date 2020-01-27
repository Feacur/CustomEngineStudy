#pragma once
#include "engine/core/types.h"

namespace custom {

struct Bytecode;

void load_image(Bytecode & bc, u32 asset_id, cstring path);
void load_imagef(Bytecode & bc, u32 asset_id, cstring path);
void load_image16(Bytecode & bc, u32 asset_id, cstring path);

}
