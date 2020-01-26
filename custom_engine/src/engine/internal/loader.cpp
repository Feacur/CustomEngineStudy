#include "custom_pch.h"
#include "engine/api/loader.h"
#include "engine/core/code.h"
#include "engine/core/math_types.h"
#include "engine/api/graphics_vm.h"
#include "engine/impl/command_buffer.h"
#include "engine/debug/log.h"

#include <stb_image.h>

namespace custom {

static Texture_Format get_texture_format(s32 value) {
	switch (value)
	{
		case 1: return Texture_Format::R;
		case 2: return Texture_Format::RG;
		case 3: return Texture_Format::RGB;
		case 4: return Texture_Format::RGBA;
	}
	CUSTOM_ASSERT(false, "not supported channels count %d", value);
	return Texture_Format::None;
}

void load_image(cstring path, u32 asset_id, Command_Buffer & command_buffer) {
	stbi_set_flip_vertically_on_load(1);

	ivec2 size;
	s32 channels;
	stbi_uc * data = stbi_load(path, &size.x, &size.y, &channels, 0);

	Texture_Format format = get_texture_format(channels);

	command_buffer.write(Graphics_Instruction::Allocate_Texture);
	command_buffer.write(asset_id);
	command_buffer.write(size);
	command_buffer.write(format);
	command_buffer.write(Data_Type::U8);

	command_buffer.write(Graphics_Instruction::Load_Texture);
	command_buffer.write(asset_id);
	command_buffer.write(size);
	command_buffer.write(format);
	command_buffer.write(Data_Type::U8);
	command_buffer.write(data, size.x * size.y * channels);

	stbi_image_free(data);
}

}
