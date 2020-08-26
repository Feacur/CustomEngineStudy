#pragma once
#include "engine/core/collection_types.h"

namespace custom {

cstring get(u32 id);
u32 get_or_add_id(cstring data, u32 length);

}
