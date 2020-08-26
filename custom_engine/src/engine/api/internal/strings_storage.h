#pragma once
#include "engine/core/collection_types.h"

namespace custom {

struct Strings_Storage {
	Array<char> values;
	Array<u32> lengths;

	void ensure_capacity(u32 values_capacity, u32 lengths_capacity);
	cstring get_string(u32 id);
	u32 store_string(cstring data, u32 length);
};

}
