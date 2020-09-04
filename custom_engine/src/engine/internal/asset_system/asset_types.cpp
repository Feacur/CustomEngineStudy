#include "custom_pch.h"

#include "engine/core/code.h"
#include "engine/debug/log.h"
#include "engine/api/internal/asset_types.h"
#include "engine/api/internal/parsing.h"
#include "engine/impl/array.h"

#include "obj_parser.h"

#include <stb_image.h>
#include <new>

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
		// new (&buffer) Mesh_Asset::Buffer;
	
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
		new (&buffer) Mesh_Asset::Buffer;

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

}

//
//
//

namespace custom {

void Prefab_Asset::update(Array<u8> & file) {
	file.push('\0'); --file.count;

	cstring source = (cstring)file.data;
	custom::Entity prefab = Entity::create(false);
	prefab.serialization_read(&source);

	entity = prefab;

	// @Note: some weird behaviour occured here, 29 August 2020;
	//       optimization related or memory related or something else, I don't grasp currently?
	//       loading partially and sporadically fails if you first store
	//       the asset pointer and immediately assign it with `*asset = {prefab}`;
	//       everything's fine, however if you do as it's done above
	//       the notorious printf()-"fix" works, too
	//       ...
	//       although, if Ref is inherited, everything seems to bee alright
	//       just be aware
}

}

//
// Config_Asset
//

namespace custom {

template struct Array<Config_Asset::Entry>;
Strings_Storage Config_Asset::strings;

template<> void Config_Asset::set_value<s32>(cstring key, s32 value) {
	u32 id = strings.store_string(key, custom::empty_index);
	for (u32 i = 0; i < entries.count; ++i) {
		if (entries[i].key == id) { entries[i].value_s32 = value; return; }
	}
	Entry * entry = (entries.push(), &entries.data[entries.count - 1]);
	entry->key = id;
	entry->type = Value_Type::s32;
	entry->value_s32 = value;
}

template<> void Config_Asset::set_value<u32>(cstring key, u32 value) {
	u32 id = strings.store_string(key, custom::empty_index);
	for (u32 i = 0; i < entries.count; ++i) {
		if (entries[i].key == id) { entries[i].value_u32 = value; return; }
	}
	Entry * entry = (entries.push(), &entries.data[entries.count - 1]);
	entry->key = id;
	entry->type = Value_Type::u32;
	entry->value_u32 = value;
}

template<> void Config_Asset::set_value<r32>(cstring key, r32 value) {
	u32 id = strings.store_string(key, custom::empty_index);
	for (u32 i = 0; i < entries.count; ++i) {
		if (entries[i].key == id) { entries[i].value_r32 = value; return; }
	}
	Entry * entry = (entries.push(), &entries.data[entries.count - 1]);
	entry->key = id;
	entry->type = Value_Type::r32;
	entry->value_r32 = value;
}

template<> void Config_Asset::set_value<bool>(cstring key, bln value) {
	u32 id = strings.store_string(key, custom::empty_index);
	for (u32 i = 0; i < entries.count; ++i) {
		if (entries[i].key == id) { entries[i].value_u32 = value ? 1 : 0; return; }
	}
	Entry * entry = (entries.push(), &entries.data[entries.count - 1]);
	entry->key = id;
	entry->type = Value_Type::bln;
	entry->value_u32 = value ? 1 : 0;
}

template<> void Config_Asset::set_value<cstring>(cstring key, cstring value) {
	u32 id = strings.store_string(key, custom::empty_index);
	for (u32 i = 0; i < entries.count; ++i) {
		if (entries[i].key == id) {
			entries[i].value_u32 = strings.store_string(value, custom::empty_index);
			return;
		}
	}
	Entry * entry = (entries.push(), &entries.data[entries.count - 1]);
	entry->key = id;
	entry->type = Value_Type::str;
	entry->value_u32 = strings.store_string(value, custom::empty_index);
}

template<> s32 Config_Asset::get_value<s32>(cstring key, s32 default_value) const {
	u32 id = strings.store_string(key, custom::empty_index);
	for (u32 i = 0; i < entries.count; ++i) {
		if (entries[i].key == id) { return entries[i].value_s32; }
	}
	return default_value;
}

template<> u32 Config_Asset::get_value<u32>(cstring key, u32 default_value) const {
	u32 id = strings.store_string(key, custom::empty_index);
	for (u32 i = 0; i < entries.count; ++i) {
		if (entries[i].key == id) { return entries[i].value_u32; }
	}
	return default_value;
}

template<> r32 Config_Asset::get_value<r32>(cstring key, r32 default_value) const {
	u32 id = strings.store_string(key, custom::empty_index);
	for (u32 i = 0; i < entries.count; ++i) {
		if (entries[i].key == id) { return entries[i].value_r32; }
	}
	return default_value;
}

template<> bln Config_Asset::get_value<bln>(cstring key, bln default_value) const {
	u32 id = strings.store_string(key, custom::empty_index);
	for (u32 i = 0; i < entries.count; ++i) {
		if (entries[i].key == id) { return (bln)entries[i].value_u32; }
	}
	return default_value;
}

template<> cstring Config_Asset::get_value<cstring>(cstring key, cstring default_value) const {
	u32 id = strings.store_string(key, custom::empty_index);
	for (u32 i = 0; i < entries.count; ++i) {
		if (entries[i].key == id) {
			return strings.get_string(entries[i].value_u32);
		}
	}
	return default_value;
}

void Config_Asset::update(Array<u8> & file) {
	file.push('\0'); --file.count;

	entries.count = 0;

	// @Todo: factor a common allocator out?
	Strings_Storage cache;

	cstring source = (cstring)file.data;
	while (*source) {
		parse_void(&source);

		if (strncmp(source, "s32", 3) == 0) { source += 3;
			cstring string_end = (parse_void(&source), source); skip_to_void(&string_end);
			u32 key = cache.store_string(source, (u32)(string_end - source));

			source = string_end;
			s32 value = (parse_void(&source), parse_s32(&source));
			set_value(cache.get_string(key), value);
		}

		if (strncmp(source, "u32", 3) == 0) { source += 3;
			cstring string_end = (parse_void(&source), source); skip_to_void(&string_end);
			u32 key = cache.store_string(source, (u32)(string_end - source));

			source = string_end;
			u32 value = (parse_void(&source), parse_u32(&source));
			set_value(cache.get_string(key), value);
		}

		if (strncmp(source, "r32", 3) == 0) { source += 3;
			cstring string_end = (parse_void(&source), source); skip_to_void(&string_end);
			u32 key = cache.store_string(source, (u32)(string_end - source));

			source = string_end;
			r32 value = (parse_void(&source), parse_r32(&source));
			set_value(cache.get_string(key), value);
		}

		if (strncmp(source, "bln", 3) == 0) { source += 3;
			cstring string_end = (parse_void(&source), source); skip_to_void(&string_end);
			u32 key = cache.store_string(source, (u32)(string_end - source));

			source = string_end;
			bln value = (parse_void(&source), parse_bln(&source));
			set_value(cache.get_string(key), value);
		}

		if (strncmp(source, "str", 3) == 0) { source += 3;
			cstring string_end = (parse_void(&source), source); skip_to_void(&string_end);
			u32 key = cache.store_string(source, (u32)(string_end - source));

			source = string_end;
			cstring line_end = (parse_void(&source), source); skip_to_eol(&line_end);
			u32 value = cache.store_string(source, (u32)(line_end - source));
			set_value(cache.get_string(key), cache.get_string(value));
		}

		skip_to_eol(&source); parse_eol(&source);
	}

	++version;
}

}
