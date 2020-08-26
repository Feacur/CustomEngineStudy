#include "custom_pch.h"
#include "engine/api/internal/strings_storage.h"

#include "engine/impl/array.h"

#include <string.h>

namespace custom {

void Strings_Storage::ensure_capacity(u32 values_capacity, u32 lengths_capacity) {
	values.ensure_capacity(values_capacity);
	lengths.ensure_capacity(lengths_capacity);
}

cstring Strings_Storage::get_string(u32 id) {
	u32 string_offset = 0;
	for (u32 i = 0; i < id; ++i) {
		string_offset += lengths[i];
	}
	return &values.data[string_offset];
}

u32 Strings_Storage::store_string(cstring value, u32 length) {
	if (length == custom::empty_index) {
		length = (u32)strlen(value);
	}

	u32 string_offset = 0;
	for (u32 i = 0; i < lengths.count; ++i) {
		u32 string_length = lengths[i];
		cstring name = &values[string_offset];
		if (strncmp(value, name, length) == 0) { return i; }
		string_offset += string_length;
	}

	u32 id = lengths.count;
	lengths.push(length + 1);
	values.push_range(value, length);
	values.push('\0');
	return id;
}

}
