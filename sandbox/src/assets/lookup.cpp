#include "engine/core/code.h"

#include "ids.h"

namespace custom {
namespace asset {

namespace uniform {
	constexpr u32 const ids_count = (u32)sandbox::Uniform::count;
	u32 count = ids_count;
	cstring names[ids_count + 1] = {
		"u_Resolution",
		"u_ViewProjection",
		"u_Transform",
		"u_Texture",
		"u_Color",
		"u_Z",
		NULL
	};
};

}}
