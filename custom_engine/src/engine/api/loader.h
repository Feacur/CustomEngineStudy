#pragma once
#include "engine/core/types.h"
#include "engine/api/command_buffer.h"

namespace custom {

void load_image(Command_Buffer & command_buffer, u32 asset_id, cstring path);
void load_imagef(Command_Buffer & command_buffer, u32 asset_id, cstring path);
void load_image16(Command_Buffer & command_buffer, u32 asset_id, cstring path);

}
