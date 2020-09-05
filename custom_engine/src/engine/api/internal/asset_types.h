#pragma once
#include "engine/core/types.h"
#include "engine/core/math_types.h"
#include "engine/core/collection_types.h"
#include "engine/api/internal/entity_system.h"
#include "engine/api/graphics_params.h"

namespace custom {

struct Shader_Asset {
	Array<u8> source;

	void update(Array<u8> & file);

	~Shader_Asset() = default;
};

struct Texture_Asset {
	Array<u8> data;
	ivec2 size;
	s32 channels;

	u8 is_dynamic;
	graphics::Data_Type data_type;
	graphics::Texture_Type texture_type;

	graphics::Filter_Mode min_tex, min_mip, mag_tex;
	graphics::Wrap_Mode wrap_x, wrap_y;

	void update(Array<u8> & file);

	~Texture_Asset() = default;
};

struct Mesh_Asset {
	struct Buffer {
		Array<u8> attributes;
		Array<u8> buffer;

		u8 is_index;
		graphics::Data_Type data_type;

		graphics::Mesh_Frequency frequency;
		graphics::Mesh_Access access;
	};
	Array<Buffer> buffers;

	void update(Array<u8> & file);

	~Mesh_Asset(); // @Note: array is POD and doesn't call elements' destructor
};

struct Collider2d_Asset {
	Array<vec2> points;

	void update(Array<u8> & file);

	~Collider2d_Asset() = default;
};

struct Prefab_Asset {
	Entity entity;

	void update(Array<u8> & file);
};

struct Config_Asset {
	static Strings_Storage strings;

	enum struct Value_Type : u8 {None, s32, u32, r32, bln, str};
	struct Entry {
		u32 key;
		union {
			s32 value_s32;
			u32 value_u32;
			r32 value_r32;
		};
		Value_Type type;
	};
	Array<Entry> entries;
	u32 version = custom::empty_index;

	template<typename T> void set_value(cstring key, T value);
	template<typename T> T get_value(cstring key, T default_value) const;

	void update(Array<u8> & file);

	~Config_Asset() = default;
};

}
