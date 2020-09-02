#pragma once
#include "engine/core/collection_types.h"

namespace custom {

struct Strings_Storage {
	Array<char> values;
	Array<u32> offsets;
	Array<u32> lengths;

	void clear(void);
	inline u32 get_count() const { return offsets.count; }

	u32 store_string(cstring data, u32 length);
	u32 get_id(cstring data, u32 length) const;

	cstring get_string(u32 id) const;
	u32 get_length(u32 id) const;
};

}
