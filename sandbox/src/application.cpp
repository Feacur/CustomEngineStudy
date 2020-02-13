#include "application.h"
#include "engine/api/system.h"
#include "engine/api/timer.h"
#include "engine/api/window.h"
#include "engine/api/loader.h"
#include "engine/api/renderer.h"
#include "engine/api/graphics_vm.h"
#include "engine/impl/bytecode.h"
#include "engine/impl/math_linear.h"
#include "entity_system/renderer.h"

#if !defined(CUSTOM_SHIPPING)
static void display_performace(custom::window::Data * window, u64 duration, u64 precision) {
	float debug_ms = duration * 1000 / (float)precision;
	float debug_fps = precision / (float)duration;
	static char header_text[64];
	sprintf(header_text, "custom engine - %.1f ms (%.1f FPS)", debug_ms, debug_fps);
	custom::window::set_header(window, header_text);
}
	#define DISPLAY_PERFORMANCE(window, duration, precision)\
		display_performace(window, duration, precision)
#else
	#define DISPLAY_PERFORMANCE(window, duration, precision)
#endif

static u64 get_last_frame_ticks(bool vsync) {
	static u64 const duration  = 16666;
	static u64 const precision = custom::timer::microsecond;
	if (vsync) {
		return custom::timer::snapshot();
	}
	return custom::timer::wait_next_frame(duration, precision);
}

namespace sandbox {
namespace application {

static custom::Bytecode gbc;
static custom::window::Data * window;
static init_func * init_callback;
static viewport_func * viewport_callback;
static update_func * update_callback;

static void init(void) {
	custom::system::init();
	custom::timer::init();
	
	window = custom::window::create();
	custom::window::init_context(window);
	custom::window::set_vsync(window, 1);

	custom::loader::init(&gbc);
	custom::renderer::init(&gbc);
	sandbox::renderer::init(&gbc);
	custom::graphics::init();

	(*init_callback)();
	
	ivec2 size = custom::window::get_size(window);
	(*viewport_callback)(size);
}

void set_init_callback(init_func * callback) {
	init_callback = callback;
}

void set_viewport_callback(viewport_func * callback) {
	viewport_callback = callback;
}

void set_update_callback(update_func * callback) {
	update_callback = callback;
}

void run(void) {
	init();

	while (true) {
		if (custom::system::should_close) { break; }
		if (custom::window::get_should_close(window)) { break; }

		// prepare for a frame
		u64 last_frame_ticks = get_last_frame_ticks(custom::window::check_vsync(window));
		DISPLAY_PERFORMANCE(window, last_frame_ticks, custom::timer::ticks_per_second);

		// process the frame
		custom::system::update();

		r32 dt = (r32)last_frame_ticks / custom::timer::ticks_per_second;
		(*update_callback)(dt);

		custom::graphics::update(gbc);
		custom::window::update(window);

		// clean up after the frame
		gbc.offset = 0;
		gbc.buffer.count = 0;
	}

	custom::graphics::shutdown();
	custom::window::destroy(window);
}

}}
