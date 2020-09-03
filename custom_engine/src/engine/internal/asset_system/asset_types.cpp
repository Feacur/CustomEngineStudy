#include "custom_pch.h"

#include "engine/core/code.h"
#include "engine/debug/log.h"
#include "engine/api/internal/asset_types.h"
#include "engine/api/internal/parsing.h"
#include "engine/impl/array.h"

//
// Mesh_Asset
//

namespace custom {

template struct Array<Mesh_Asset::Buffer>;

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

void Config_Asset::update(cstring source) {
	Strings_Storage cache;

	entries.count = 0;
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
