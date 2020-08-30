#include "custom_pch.h"

#include "engine/core/code.h"
#include "engine/debug/log.h"
#include "engine/api/platform/system.h"
#include "engine/api/platform/timer.h"
#include "engine/api/platform/window.h"
#include "engine/api/platform/graphics_vm.h"
#include "engine/api/internal/application.h"
#include "engine/api/internal/bytecode.h"
#include "engine/api/internal/loader.h"
#include "engine/api/internal/renderer.h"
#include "engine/impl/math_linear.h"

#define APP_DISPLAY_PERFORMANCE

#if defined(CUSTOM_SHIPPING)
	// #undef APP_DISPLAY_PERFORMANCE
#endif

#if defined(APP_DISPLAY_PERFORMANCE)
	static void DISPLAY_PERFORMANCE(
		custom::window::Internal_Data * window,
		u64 time_system, u64 time_logic, u64 time_render, u64 precision,
		r32 dt
	) {
		u64 const duration = (time_system + time_logic + time_render);
		float const system_ms = time_system * custom::timer::millisecond / (float)precision;
		float const logic_ms  = time_logic  * custom::timer::millisecond / (float)precision;
		float const render_ms = time_render * custom::timer::millisecond / (float)precision;
		float const debug_ms  = duration * custom::timer::millisecond / (float)precision;
		float const debug_fps = precision / (float)duration;
		float const debug_dt  = dt * custom::timer::millisecond;
		static char header_text[128];
		s32 header_length = sprintf(
			header_text,
			"custom engine"
			"- %.1f ms (%.1f FPS)"
			"---> system: %.1f ms | logic: %.1f ms | render: %.1f ms"
			"---> dt %.1f ms",
			debug_ms, debug_fps,
			system_ms, logic_ms, render_ms,
			debug_dt
		);
		CUSTOM_ASSERT(header_length >= 0 && header_length <= C_ARRAY_LENGTH(header_text), "out of bounds");
		custom::window::set_header(window, header_text);
	}
#else
	#define DISPLAY_PERFORMANCE(window, time_system, time_logic, time_render, precision, dt) (void)0
#endif

static u64 get_last_frame_ticks(bool vsync, u16 refresh_rate) {
	if (vsync) { return custom::timer::snapshot(); }
	static u64 const precision = custom::timer::nanosecond;
	return custom::timer::wait_next_frame(precision / (u64)refresh_rate, precision);
}

void init_asset_types(void);
void init_component_types(void);

namespace custom {
namespace application {

static struct {
	custom::Bytecode bytecode_loader;
	custom::Bytecode bytecode_renderer;
	custom::window::Internal_Data * window;

	ivec2 viewport_size;

	struct {
		u16 target;
		u8 failsafe;
		u8 vsync;
		b8 force;
	} refresh_rate;

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
	custom::loader::init(&app.bytecode_loader);
	custom::renderer::init(&app.bytecode_renderer);

	init_asset_types();
	init_component_types();

	// @Todo: init context outside a window environment?
	app.window = custom::window::create();
	custom::window::init_context(app.window);

	ivec2 size = custom::window::get_size(app.window);

	// @Todo: expose vsync setting
	custom::window::set_vsync(app.window, app.refresh_rate.vsync);
	custom::window::set_viewport_callback(app.window, &update_viewport_safely);

	update_viewport_safely(app.window, size);
	CALL_SAFELY(app.callbacks.init);
}

void run(void) {
	init();

	custom::timer::snapshot();

	while (true) {
		if (custom::system::should_close) { break; }
		if (custom::window::get_should_close(app.window)) { break; }

		u64 time_system = custom::timer::get_ticks();
		custom::window::update(app.window);
		custom::system::update();
		time_system = custom::timer::get_ticks() - time_system;

		// prepare for a frame
		u16 refresh_rate = app.refresh_rate.force
			? app.refresh_rate.target
			: custom::window::get_refresh_rate(app.window, app.refresh_rate.target);
		u64 last_frame_ticks = get_last_frame_ticks(
			custom::window::check_vsync(app.window),
			refresh_rate
		);

		r32 dt = (r32)last_frame_ticks / custom::timer::ticks_per_second;
		if (dt > (r32)app.refresh_rate.failsafe / refresh_rate) { dt = 1.0f / refresh_rate; }

		// process the frame
		u64 time_logic = custom::timer::get_ticks();
		CALL_SAFELY(app.callbacks.update, dt);
		time_logic = custom::timer::get_ticks() - time_logic;

		u64 time_render = custom::timer::get_ticks();
		custom::graphics::consume(app.bytecode_loader);
		custom::graphics::consume(app.bytecode_renderer);
		app.bytecode_loader.reset();
		app.bytecode_renderer.reset();
		time_render = custom::timer::get_ticks() - time_render;

		DISPLAY_PERFORMANCE(
			app.window,
			time_system, time_logic, time_render,
			custom::timer::ticks_per_second,
			dt
		);
	}

	custom::timer::shutdown();
	custom::graphics::shutdown();
	custom::window::destroy(app.window);
}

void set_refresh_rate(u16 target, u8 failsafe, u8 vsync, b8 force) {
	app.refresh_rate = {target, failsafe, vsync, force};
	if (app.window) { custom::window::set_vsync(app.window, app.refresh_rate.vsync); }
}

// data
ivec2 const & get_viewport_size(void) {
	return app.viewport_size;
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

ivec2 const & get_mouse_pos(void) {
	return custom::window::get_mouse_pos(app.window);
}

ivec2 const & get_mouse_delta(void) {
	return custom::window::get_mouse_delta(app.window);
}

vec2 const & get_mouse_wheel(void) {
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
