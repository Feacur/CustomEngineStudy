#include "custom_pch.h"

#include "engine/core/code.h"
#include "engine/core/collection_types.h"
#include "engine/debug/log.h"
#include "engine/api/rendering_settings.h"

#include <string.h>

namespace custom {
namespace system {
	bool should_close;
}}

namespace custom {
namespace timer {
	u64 ticks_per_second;
	u64 frame_start_ticks;
}}

namespace custom {
	Context_Settings context_settings;
	Pixel_Format pixel_format_hint;
}

namespace custom {
	Array<cstring> component_names;
	Array<cstring> asset_names;
	Array<cstring> uniform_names;
}

namespace custom {
	// @Todo: store strings
	static Array<char> todo_strings;
	static Array<u32> todo_strings_lengths;

	cstring get(u32 id) {
		u32 string_offset = 0;
		for (u32 i = 0; i < id; ++i) {
			string_offset += todo_strings_lengths[i];
		}
		return &todo_strings.data[string_offset];
	}

	u32 get_or_add_id(cstring data, u32 length) {
		if (length == custom::empty_index) {
			length = (u32)strlen(data);
		}

		u32 string_offset = 0;
		for (u32 i = 0; i < todo_strings_lengths.count; ++i) {
			u32 string_length = todo_strings_lengths[i];
			cstring name = &todo_strings[string_offset];
			if (strncmp(data, name, length) == 0) { return i; }
			string_offset += string_length;
		}

		u32 id = todo_strings_lengths.count;
		todo_strings_lengths.push(length + 1);
		todo_strings.push_range(data, length);
		todo_strings.push('\0');
		return id;
	}
}
