#include "custom_pch.h"
#include "engine/api/internal/strings_storage.h"

#include "engine/impl/array.h"

#include <string.h>

namespace custom {

void Strings_Storage::clear() {
	values.count  = 0;
	offsets.count = 0;
	lengths.count = 0;
}

u32 Strings_Storage::store_string(cstring value, u32 length) {
	if (length == custom::empty_index) {
		length = (u32)strlen(value);
	}

	for (u32 i = 0; i < offsets.count; ++i) {
		u32 string_offset = offsets[i];
		cstring name = &values[string_offset];
		if (lengths[i] != length) { continue; }
		if (strncmp(value, name, length) == 0) { return i; }
	}

	u32 id = offsets.count;
	lengths.push(length);
	offsets.push(values.count);
	values.push_range(value, length);
	values.push('\0');
	return id;
}

u32 Strings_Storage::get_id(cstring value, u32 length) {
	if (length == custom::empty_index) {
		length = (u32)strlen(value);
	}

	for (u32 i = 0; i < offsets.count; ++i) {
		u32 string_offset = offsets[i];
		cstring name = &values[string_offset];
		if (lengths[i] != length) { continue; }
		if (strncmp(value, name, length) == 0) { return i; }
	}

	return custom::empty_index;
}

cstring Strings_Storage::get_string(u32 id) {
	u32 string_offset = offsets[id];
	return &values.data[string_offset];
}

u32 Strings_Storage::get_length(u32 id) {
	return lengths[id];
}

}
