#include "custom_pch.h"

#include "engine/core/collection_types.h"
#include "engine/api/internal/types_names_lookup.h"
#include "engine/api/rendering_settings.h"

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
	Strings_Storage component_names;
	Strings_Storage asset_names;
	Array<cstring> uniform_names;
}

namespace custom {
namespace file {
	Strings_Storage modified;
}}
