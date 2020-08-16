#include "custom_pch.h"

#include "engine/core/collection_types.h"
#include "engine/api/platform/system.h"
#include "engine/api/platform/timer.h"
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
	Array<cstring> component_names;
	Array<cstring> asset_names;
	Array<cstring> uniform_names;
}

namespace custom {
namespace globals {

void init(void) {
	// context_settings
	context_settings = {};
	context_settings.major_version = 4;
	context_settings.minor_version = 6;

	// pixel_format_hint
	pixel_format_hint = {};
	pixel_format_hint.red_bits     = 8;
	pixel_format_hint.green_bits   = 8;
	pixel_format_hint.blue_bits    = 8;
	pixel_format_hint.alpha_bits   = 8;
	pixel_format_hint.depth_bits   = 24;
	pixel_format_hint.stencil_bits = 8;
	pixel_format_hint.doublebuffer = true;
}

}}
