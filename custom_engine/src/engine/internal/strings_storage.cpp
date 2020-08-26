#include "custom_pch.h"
#include "engine/api/internal/strings_storage.h"

#include "engine/impl/array.h"

#include <string.h>

namespace custom {
	cstring Strings_Storage::get_string(u32 id) {
		u32 string_offset = 0;
		for (u32 i = 0; i < id; ++i) {
			string_offset += values_lengths[i];
		}
		return &values.data[string_offset];
	}

	u32 Strings_Storage::store_string(cstring data, u32 length) {
		if (length == custom::empty_index) {
			length = (u32)strlen(data);
		}

		u32 string_offset = 0;
		for (u32 i = 0; i < values_lengths.count; ++i) {
			u32 string_length = values_lengths[i];
			cstring name = &values[string_offset];
			if (strncmp(data, name, length) == 0) { return i; }
			string_offset += string_length;
		}

		u32 id = values_lengths.count;
		values_lengths.push(length + 1);
		values.push_range(data, length);
		values.push('\0');
		return id;
	}
}
