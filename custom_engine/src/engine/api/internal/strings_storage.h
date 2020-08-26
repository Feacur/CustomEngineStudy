#pragma once
#include "engine/core/collection_types.h"

namespace custom {

struct Strings_Storage {
	Array<char> values;
	Array<u32> values_lengths;

	cstring get_value(u32 id);
	u32 get_or_add_id(cstring data, u32 length);
};

extern Strings_Storage strings_storage;

}
