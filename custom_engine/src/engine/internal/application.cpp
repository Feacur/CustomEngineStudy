#include "custom_pch.h"

#include "engine/api/platform/system.h"
#include "engine/api/platform/timer.h"
#include "engine/api/platform/window.h"
#include "engine/api/platform/graphics_vm.h"
#include "engine/api/internal/application.h"
#include "engine/api/internal/loader.h"
#include "engine/api/internal/renderer.h"
#include "engine/impl/bytecode.h"
#include "engine/impl/math_linear.h"

#if !defined(CUSTOM_SHIPPING)
	#define APP_DISPLAY_PERFORMANCE
#endif

#if defined(APP_DISPLAY_PERFORMANCE)
	static void DISPLAY_PERFORMANCE(custom::window::Internal_Data * window, u64 duration, u64 precision) {
		float debug_ms = duration * 1000 / (float)precision;
		float debug_fps = precision / (float)duration;
		static char header_text[64];
		sprintf(header_text, "custom engine - %.1f ms (%.1f FPS)", debug_ms, debug_fps);
		custom::window::set_header(window, header_text);
	}
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

namespace custom {
namespace application {

static struct {
	custom::Bytecode loader_gbc;
	custom::Bytecode renderer_gbc;
	custom::window::Internal_Data * window;

	ivec2 viewport_size;

	struct {
		init_func * init;
		viewport_func * viewport;
		update_func * update;
	} callbacks;
} app;

static void update_viewport_safely(custom::window::Internal_Data * window, ivec2 size) {
	if (size == ivec2{0, 0}) { return; }
	if (app.viewport_size == size) { return; }
	app.viewport_size = size;
	CALL_SAFELY(app.callbacks.viewport, size);
}

static void init(void) {
	custom::system::init();
	custom::timer::init();
	custom::loader::init(&app.loader_gbc);
	custom::renderer::init(&app.renderer_gbc);

	// @Todo: init context outside a window environment?
	app.window = custom::window::create();
	custom::window::init_context(app.window);

	ivec2 size = custom::window::get_size(app.window);

	// @Todo: expose vsync setting
	custom::window::set_vsync(app.window, 1);
	custom::window::set_viewport_callback(app.window, &update_viewport_safely);

	update_viewport_safely(app.window, size);
	CALL_SAFELY(app.callbacks.init, &app.loader_gbc, &app.renderer_gbc);
}

void run(void) {
	init();

	while (true) {
		if (custom::system::should_close) { break; }
		if (custom::window::get_should_close(app.window)) { break; }

		// prepare for a frame
		u64 last_frame_ticks = get_last_frame_ticks(custom::window::check_vsync(app.window));
		DISPLAY_PERFORMANCE(app.window, last_frame_ticks, custom::timer::ticks_per_second);

		// process the frame
		custom::system::update();

		r32 dt = (r32)last_frame_ticks / custom::timer::ticks_per_second;
		CALL_SAFELY(app.callbacks.update, dt);

		custom::graphics::update(app.loader_gbc);
		custom::graphics::update(app.renderer_gbc);
		custom::window::update(app.window);

		// clean up after the frame
		app.loader_gbc.reset();
		app.renderer_gbc.reset();
	}

	custom::graphics::shutdown();
	custom::window::destroy(app.window);
}

// input
bool get_key(Key_Code key) {
	return custom::window::get_key(app.window, key);
}

bool get_mouse_key(Mouse_Code key) {
	return custom::window::get_mouse_key(app.window, key);
}

bool get_key_transition(Key_Code key, bool to_state) {
	return custom::window::get_key_transition(app.window, key, to_state);
}

bool get_mouse_key_transition(Mouse_Code key, bool to_state) {
	return custom::window::get_mouse_key_transition(app.window, key, to_state);
}

ivec2 const & get_mouse_pos() {
	return custom::window::get_mouse_pos(app.window);
}

ivec2 const & get_mouse_delta() {
	return custom::window::get_mouse_delta(app.window);
}

vec2 const & get_mouse_wheel() {
	return custom::window::get_mouse_wheel(app.window);
}

// callbacks
void set_init_callback(init_func * callback) {
	app.callbacks.init = callback;
}

void set_viewport_callback(viewport_func * callback) {
	app.callbacks.viewport = callback;
}

void set_update_callback(update_func * callback) {
	app.callbacks.update = callback;
}

}}
