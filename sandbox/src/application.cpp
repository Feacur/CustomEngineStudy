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
static custom::window::Data * app_window;
static struct {
	init_func * init;
	viewport_func * viewport;
	update_func * update;
} callbacks;

static void impl_viewport(custom::window::Data * window, ivec2 size) {
	custom::renderer::viewport({0, 0}, size);
	(*callbacks.viewport)(size);
}

static void init(void) {
	custom::system::init();
	custom::timer::init();
	
	app_window = custom::window::create();
	custom::window::set_viewport_callback(app_window, &impl_viewport);
	custom::window::init_context(app_window);
	custom::window::set_vsync(app_window, 1);

	custom::loader::init(&gbc);
	custom::renderer::init(&gbc);
	sandbox::renderer::init(&gbc);
	custom::graphics::init();

	(*callbacks.init)();
	
	ivec2 size = custom::window::get_size(app_window);
	custom::renderer::viewport({0, 0}, size);
	(*callbacks.viewport)(size);
}

void run(void) {
	init();

	while (true) {
		if (custom::system::should_close) { break; }
		if (custom::window::get_should_close(app_window)) { break; }

		// prepare for a frame
		u64 last_frame_ticks = get_last_frame_ticks(custom::window::check_vsync(app_window));
		DISPLAY_PERFORMANCE(app_window, last_frame_ticks, custom::timer::ticks_per_second);

		// process the frame
		custom::system::update();

		r32 dt = (r32)last_frame_ticks / custom::timer::ticks_per_second;
		(*callbacks.update)(dt);

		custom::graphics::update(gbc);
		custom::window::update(app_window);

		// clean up after the frame
		gbc.offset = 0;
		gbc.buffer.count = 0;
	}

	custom::graphics::shutdown();
	custom::window::destroy(app_window);
}

// input
custom::Key_State get_key(custom::Key_Code key) {
	return custom::window::get_key(app_window, key);
}

custom::Key_State get_mouse_key(custom::Mouse_Code key) {
	return custom::window::get_mouse_key(app_window, key);
}

ivec2 const & get_mouse_pos() {
	return custom::window::get_mouse_pos(app_window);
}

ivec2 const & get_mouse_delta() {
	return custom::window::get_mouse_delta(app_window);
}

vec2 const & get_mouse_wheel() {
	return custom::window::get_mouse_wheel(app_window);
}

// callbacks
void set_init_callback(init_func * callback) {
	callbacks.init = callback;
}

void set_viewport_callback(viewport_func * callback) {
	callbacks.viewport = callback;
}

void set_update_callback(update_func * callback) {
	callbacks.update = callback;
}

}}
