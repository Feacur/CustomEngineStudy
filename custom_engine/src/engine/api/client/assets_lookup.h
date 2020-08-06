#pragma once
#include "engine/api/asset_structs.h"

namespace custom {
namespace asset {

namespace script {
	extern u32 count;
	extern cstring paths[];
};

namespace shader {
	extern u32 meta_presets_count;
	extern Meta meta_presets[];

	extern u32 count;
	extern cstring paths[];
	extern u8 meta_ids[];
};

namespace uniform {
	extern u32 count;
	extern cstring names[];
};

namespace texture {
	extern u32 meta_presets_count;
	extern Meta meta_presets[];

	extern u32 count;
	extern cstring paths[];
	extern u8 meta_ids[];
};

namespace mesh {
	extern u32 meta_presets_count;
	extern Meta meta_presets[];

	extern u32 count;
	extern cstring paths[];
	extern u8 meta_ids[];
};

namespace target {
	extern u32 meta_presets_count;
	extern Meta meta_presets[];

	extern u32 count;
	extern u8 meta_ids[];
};

}}
