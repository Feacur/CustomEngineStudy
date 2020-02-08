#include "custom_pch.h"

#include "engine/core/types.h"
#include "engine/api/system.h"
#include "engine/api/timer.h"
#include "engine/api/window.h"
#include "engine/api/rendering_settings.h"
#include "engine/impl/array.h"

namespace custom {

System system = {};
Timer timer = {};
bool Window::should_close = false;

Context_Settings context_settings;
Pixel_Format pixel_format_hint;

void globals_init(void) {
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

}
