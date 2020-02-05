#pragma once
#include "engine/core/collection_types.h"
#include "engine/api/graphics_params.h"

namespace custom {
namespace runtime {

struct Buffer
{
	b8 is_index;
	graphics::Mesh_Frequency frequency; graphics::Mesh_Access access;
	graphics::Data_Type data_type; u32 capacity; u32 count;
	Array_Fixed<u8, 4> attributes;
};

}}
