#pragma once
#include "engine/core/types.h"
#include "engine/api/command_buffer.h"

namespace custom {

void load_image(cstring path, u32 asset_id, Command_Buffer & command_buffer);

}
