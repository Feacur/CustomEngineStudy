#pragma once
#include "engine/core/collection_types.h"

namespace custom {

struct Strings_Storage {
	Array<char> values;
	Array<u32> offsets;

	void clear(void);
	void ensure_capacity(u32 values_capacity, u32 offsets_capacity);
	inline u32 get_count() { return offsets.count; }

	u32 store_string(cstring data, u32 length);
	u32 get_id(cstring data, u32 length);

	cstring get_string(u32 id);
};

}
