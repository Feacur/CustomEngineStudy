#include "custom_pch.h"

#include "engine/core/code.h"
#include "engine/debug/log.h"
#include "engine/api/internal/asset_types.h"
#include "engine/api/internal/parsing.h"
#include "engine/impl/array.h"
#include "engine/impl/parsing.h"

#include "obj_parser.h"

#include <stb_image.h>

//
// Shader_Asset
//

namespace custom {

void Shader_Asset::update(Array<u8> & file) {
	file.push('\0'); --file.count;
	source.data     = file.data;     file.data     = NULL;
	source.capacity = file.capacity; file.capacity = 0;
	source.count    = file.count;    file.count    = 0;
}

}

//
// Texture_Asset
//

namespace custom {

void Texture_Asset::update(Array<u8> & file) {
	// @Todo: read meta or provide these otherwise
	is_dynamic = false;
	data_type = graphics::Data_Type::u8;
	texture_type = graphics::Texture_Type::Color;

	min_tex = graphics::Filter_Mode::None;
	min_mip = graphics::Filter_Mode::None;
	mag_tex = graphics::Filter_Mode::None;
	wrap_x = graphics::Wrap_Mode::Repeat;
	wrap_y = graphics::Wrap_Mode::Repeat;

	u8 data_type_size = 0;

	stbi_set_flip_vertically_on_load(1);
	switch (data_type)
	{
		case custom::graphics::Data_Type::u8:
			data_type_size = sizeof(u8);
			data.data = (u8 *)stbi_load_from_memory(file.data, file.count, &size.x, &size.y, &channels, 0);
			break;
		case custom::graphics::Data_Type::u16:
			data_type_size = sizeof(u16);
			data.data = (u8 *)stbi_load_16_from_memory(file.data, file.count, &size.x, &size.y, &channels, 0);
			break;
		case custom::graphics::Data_Type::r32:
			data_type_size = sizeof(r32);
			data.data = (u8 *)stbi_loadf_from_memory(file.data, file.count, &size.x, &size.y, &channels, 0);
			break;
		default:
			CUSTOM_ASSERT(false, "texture data type is not supported: %d", (u8)data_type);
			break;
	}

	data.capacity = size.x * size.y * channels * data_type_size;
	data.count = data.capacity;
}

}

//
// Mesh_Asset
//

namespace custom {

template struct Array<Mesh_Asset::Buffer>;

void Mesh_Asset::update(Array<u8> & file) {
	file.push('\0'); --file.count;

	Array<u8> attributes;
	Array<r32> vertices;
	Array<u32> indices;
	obj::parse(file, attributes, vertices, indices);
	if (!attributes.count) { CUSTOM_ASSERT(false, "mesh has no attributes"); return; }
	if (!vertices.count) { CUSTOM_ASSERT(false, "mesh has no vertices"); return; }
	if (!indices.count) { CUSTOM_ASSERT(false, "mesh has no indices"); return; }
	
	buffers.set_capacity(2);

	{
		buffers.push();
		Mesh_Asset::Buffer & buffer = buffers[0];
	
		buffer.attributes.data     = attributes.data;     attributes.data     = NULL;
		buffer.attributes.capacity = attributes.capacity; attributes.capacity = 0;
		buffer.attributes.count    = attributes.count;    attributes.count    = 0;

		buffer.buffer.data     = (u8 *)vertices.data;             vertices.data     = NULL;
		buffer.buffer.capacity = vertices.capacity * sizeof(r32); vertices.capacity = 0;
		buffer.buffer.count    = vertices.count * sizeof(r32);    vertices.count    = 0;

		buffer.data_type = graphics::Data_Type::r32;
		buffer.is_index = false;

		// @Todo: read meta or provide these otherwise
		buffer.frequency = graphics::Mesh_Frequency::Static;
		buffer.access = graphics::Mesh_Access::Draw;
	}

	{
		buffers.push();
		Mesh_Asset::Buffer & buffer = buffers[1];
	
		buffer.attributes.data     = NULL;
		buffer.attributes.capacity = 0;
		buffer.attributes.count    = 0;

		buffer.buffer.data     = (u8 *)indices.data;             indices.data     = NULL;
		buffer.buffer.capacity = indices.capacity * sizeof(u32); indices.capacity = 0;
		buffer.buffer.count    = indices.count * sizeof(u32);    indices.count    = 0;

		buffer.data_type = graphics::Data_Type::u32;
		buffer.is_index = true;

		// @Todo: read meta or provide these otherwise
		buffer.frequency = graphics::Mesh_Frequency::Static;
		buffer.access = graphics::Mesh_Access::Draw;
	}
}

Mesh_Asset::~Mesh_Asset() {
	for (u32 i = 0; i < buffers.count; ++i) {
		buffers[i].attributes.~Array();
		buffers[i].buffer.~Array();
	}
	buffers.~Array();
}

}

//
// Collider2d_Asset
//

namespace custom {

void Collider2d_Asset::update(Array<u8> & file) {
	file.push('\0'); --file.count;

	cstring source;

	u32 vertices_capacity = 0;
	source = (cstring)file.data;
	while (*source) {
		++vertices_capacity;
		to_next_line(&source);
	}

	Array<vec2> vertices(vertices_capacity);
	source = (cstring)file.data;
	while (*source) {
		vertices.push(parse_vec2(&source));
		to_next_line(&source);
	}

	if (!vertices.count) { CUSTOM_ASSERT(false, "mesh has no vertices"); return; }

	points.set_capacity(vertices.count);
	points.data     = vertices.data;     vertices.data     = NULL;
	points.capacity = vertices.capacity; vertices.capacity = 0;
	points.count    = vertices.count;    vertices.count    = 0;
}

}

//
// Prefab_Asset
//

namespace custom {

}

//
// Config_Asset
//

namespace custom {

template struct Array<Config_Asset::Entry>;
Strings_Storage Config_Asset::strings;

#define SET_VALUE_IMPL(T)                                                   \
template<> void Config_Asset::set_value<T>(cstring key, T value) {          \
    u32 id = strings.store_string(key, custom::empty_index);                \
    for (u32 i = 0; i < entries.count; ++i) {                               \
        if (entries[i].type != Config_Asset::Value_Type::T) { continue; }   \
        if (entries[i].key == id) { entries[i].value_##T = value; return; } \
    }                                                                       \
    Entry * entry = (entries.push(), &entries.data[entries.count - 1]);     \
    entry->key       = id;                                                  \
    entry->type      = Value_Type::T;                                       \
    entry->value_##T = value;                                               \
}                                                                           \

SET_VALUE_IMPL(s32)
SET_VALUE_IMPL(u32)
SET_VALUE_IMPL(r32)
SET_VALUE_IMPL(bln)
#undef SET_VALUE_IMPL

template<> void Config_Asset::set_value<cstring>(cstring key, cstring value) {
	u32 id = strings.store_string(key, custom::empty_index);
	for (u32 i = 0; i < entries.count; ++i) {
		if (entries[i].type != Config_Asset::Value_Type::str) { continue; }
		if (entries[i].key == id) {
			entries[i].value_str = strings.store_string(value, custom::empty_index);
			return;
		}
	}
	Entry * entry = (entries.push(), &entries.data[entries.count - 1]);
	entry->key = id;
	entry->type = Value_Type::str;
	entry->value_str = strings.store_string(value, custom::empty_index);
}

//
#define GET_VALUE_IMPL(T)                                                     \
template<> T Config_Asset::get_value<T>(cstring key, T default_value) const { \
    u32 id = strings.store_string(key, custom::empty_index);                  \
    for (u32 i = 0; i < entries.count; ++i) {                                 \
        if (entries[i].type != Config_Asset::Value_Type::T) { continue; }     \
        if (entries[i].key == id) { return entries[i].value_##T; }            \
    }                                                                         \
    CUSTOM_WARNING("config doesn't contain key '%s : " #T "'", key);          \
    CUSTOM_WARNING("... using default value of '%g'", (r32)default_value);    \
    return default_value;                                                     \
}                                                                             \

GET_VALUE_IMPL(s32)
GET_VALUE_IMPL(u32)
GET_VALUE_IMPL(r32)
GET_VALUE_IMPL(bln)
#undef GET_VALUE_IMPL

template<> cstring Config_Asset::get_value<cstring>(cstring key, cstring default_value) const {
	u32 id = strings.store_string(key, custom::empty_index);
	for (u32 i = 0; i < entries.count; ++i) {
		if (entries[i].type != Config_Asset::Value_Type::str) { continue; }
		if (entries[i].key == id) { return strings.get_string(entries[i].value_str); }
	}
	CUSTOM_WARNING("config doesn't contain key '%s : str'", key);
	CUSTOM_WARNING("... using default value of '%s'", default_value);
	return default_value;
}

//
void Config_Asset::update(Array<u8> & file) {
	file.push('\0'); --file.count;

	entries.count = 0;

	// @Todo: factor a common allocator out?
	Strings_Storage cache;

	cstring source = (cstring)file.data;
	while (*source) {
		parse_void(&source);

		if (*source == '#') { to_next_line(&source); continue; }

		u32     type_length = to_identifier_length(&source);
		cstring type        = source;
		skip_to_void(&source);

		u32 key_length = to_identifier_length(&source);
		u32 key_id     = cache.store_string(source, key_length);
		skip_to_void(&source);

		if (false) { /**/ }
		else if (strncmp_auto(type, "s32") == 0) {
			s32 value = to_s32(&source);
			set_value(cache.get_string(key_id), value);
		}
		else if (strncmp_auto(type, "u32") == 0) {
			u32 value = to_u32(&source);
			set_value(cache.get_string(key_id), value);
		}
		else if (strncmp_auto(type, "r32") == 0) {
			r32 value = to_r32(&source);
			set_value(cache.get_string(key_id), value);
		}
		else if (strncmp_auto(type, "bln") == 0) {
			bln value = to_bln(&source);
			set_value(cache.get_string(key_id), value);
		}
		else if (strncmp_auto(type, "str") == 0) {
			u32 value_length = to_string_length(&source);
			u32 value_id     = cache.store_string(source, value_length);
			set_value(cache.get_string(key_id), cache.get_string(value_id));
		}

		to_next_line(&source);
	}

	++version;
}

}
