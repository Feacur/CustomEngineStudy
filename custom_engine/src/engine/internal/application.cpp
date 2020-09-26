#include "custom_pch.h"

#include "engine/core/code.h"
#include "engine/debug/log.h"
#include "engine/api/internal/asset_system.h"
#include "engine/api/internal/asset_types.h"
#include "engine/api/platform/system.h"
#include "engine/api/platform/timer.h"
#include "engine/api/platform/window.h"
#include "engine/api/platform/file.h"
#include "engine/api/platform/graphics_vm.h"
#include "engine/api/internal/application.h"
#include "engine/api/internal/bytecode.h"
#include "engine/api/internal/loader.h"
#include "engine/api/internal/renderer.h"
#include "engine/api/rendering_settings.h"
#include "engine/impl/math_linear.h"

#define APP_DISPLAY_PERFORMANCE

#if defined(CUSTOM_SHIPPING)
	// #undef APP_DISPLAY_PERFORMANCE
#endif

#if defined(APP_DISPLAY_PERFORMANCE)
	static void DISPLAY_PERFORMANCE(
		custom::window::Internal_Data * window,
		u64 time_frame,
		u64 time_system, u64 time_logic, u64 time_render,
		u64 precision, r32 dt
	) {
		r32 const frame_ms  = time_frame  * custom::timer::millisecond / (r32)precision;
		r32 const system_ms = time_system * custom::timer::millisecond / (r32)precision;
		r32 const logic_ms  = time_logic  * custom::timer::millisecond / (r32)precision;
		r32 const render_ms = time_render * custom::timer::millisecond / (r32)precision;
		r32 const frame_ps  = precision / (r32)time_frame;
		r32 const dt_ms     = dt * custom::timer::millisecond;
		static char header_text[128];
		s32 header_length = sprintf(
			header_text,
			"custom engine"
			"- %.1f ms (%.1f FPS)"
			"---> system: %.1f ms | logic: %.1f ms | render: %.1f ms"
			"---> dt %.1f ms",
			frame_ms, frame_ps,
			system_ms, logic_ms, render_ms,
			dt_ms
		);
		CUSTOM_ASSERT(header_length >= 0 && header_length <= (s32)C_ARRAY_LENGTH(header_text), "out of bounds");
		custom::window::set_header(window, header_text);
	}
#else
	#define DISPLAY_PERFORMANCE(window, time_frame, time_system, time_logic, time_render, precision, dt) (void)0
#endif

static u64 wait_till_next_frame(u64 frame_start_ticks, u16 refresh_rate, bool vsync, bool sleep_while_waiting) {
	constexpr static u64 const precision = custom::timer::nanosecond;
	if (!vsync) {
		if (sleep_while_waiting) {
			custom::timer::sleep_till_next_frame(frame_start_ticks, precision / (u64)refresh_rate, precision);
		}
		else {
			custom::timer::idle_till_next_frame(frame_start_ticks, precision / (u64)refresh_rate, precision);
		}
	}
	return custom::timer::get_ticks() - frame_start_ticks;
}

void init_asset_types(void);
void init_component_types(void);

namespace custom {
namespace application {

static struct {
	custom::Bytecode bytecode_loader;
	custom::Bytecode bytecode_renderer;
	custom::window::Internal_Data * window;

	u64 frame_start_ticks;

	ivec2 viewport_size;
	bln sleep_while_waiting         = true;
	bln update_assets_automatically = true;

	struct {
		u16 target    = 144;
		u8 debug      = 20;
		u8 failsafe   = 10;
		u8 vsync      = 1;
		b8 as_display = true;
	} refresh_rate;

	struct {
		init_func     * init;
		viewport_func * viewport;
		update_func   * update;
		close_func    * close;
	} callbacks;
} app;

static void update_viewport_safely(custom::window::Internal_Data * window, ivec2 size) {
	if (size == ivec2{0, 0}) { return; }
	if (app.viewport_size == size) { return; }
	app.viewport_size = size;
	CALL_SAFELY(app.callbacks.viewport, size);
}

static void process_close_safely(custom::window::Internal_Data * window) {
	if (!app.callbacks.close || (*app.callbacks.close)()) {
		custom::system::should_close = true;
	}
}

static custom::Asset_RefT<custom::Config_Asset> config_ref = {custom::empty_ref, custom::empty_index};

static void consume_config_init(void) {
	custom::Config_Asset const * config = config_ref.ref.get_safe();
	CUSTOM_ASSERT(config, "no config");

	u32 ogl_version = config->get_value<u32>("open_gl_version", 46);

	// context_settings
	custom::context_settings = {};
	custom::context_settings.major_version = ogl_version / 10;
	custom::context_settings.minor_version = ogl_version % 10;

	// pixel_format_hint
	custom::pixel_format_hint = {};
	custom::pixel_format_hint.red_bits     = config->get_value<u32>("pixel_format_red_bits",     8);
	custom::pixel_format_hint.green_bits   = config->get_value<u32>("pixel_format_green_bits",   8);
	custom::pixel_format_hint.blue_bits    = config->get_value<u32>("pixel_format_blue_bits",    8);
	custom::pixel_format_hint.alpha_bits   = config->get_value<u32>("pixel_format_alpha_bits",   8);
	custom::pixel_format_hint.depth_bits   = config->get_value<u32>("pixel_format_depth_bits",   8);
	custom::pixel_format_hint.stencil_bits = config->get_value<u32>("pixel_format_stencil_bits", 24);
	custom::pixel_format_hint.doublebuffer = config->get_value<bln>("pixel_format_doublebuffer", true);
	custom::pixel_format_hint.swap         = config->get_value<u32>("pixel_format_swap",         1);
}

static void consume_config(void) {
	u32 version = custom::empty_index;

	custom::Config_Asset const * config = config_ref.ref.get_safe();
	CUSTOM_ASSERT(config, "no config");

	if (version == config->version) { return; }
	version = config->version;

	app.sleep_while_waiting         = config->get_value<bln>("sleep_while_waiting", false);
	app.update_assets_automatically = config->get_value<bln>("update_assets_automatically", true);
}

static void init(void) {
	custom::system::init();
	custom::timer::init();

	//
	init_asset_types();
	init_component_types();

	custom::loader::init(&app.bytecode_loader);
	custom::renderer::init(&app.bytecode_renderer);

	u32 config_id = Asset::store_string("assets/configs/engine.cfg", custom::empty_index);
	config_ref = Asset::add<Config_Asset>(config_id);

	consume_config_init();
	consume_config();

	//
	app.window = custom::window::create();
	custom::window::set_viewport_callback(app.window, &update_viewport_safely);
	custom::window::set_close_callback(app.window, &process_close_safely);
	custom::window::init_context(app.window);

	//
	app.frame_start_ticks = custom::timer::get_ticks();
	update_viewport_safely(app.window, custom::window::get_size(app.window));
	CALL_SAFELY(app.callbacks.init);
}

void run(void) {
	static bool is_running = false;
	if (is_running) { CUSTOM_ASSERT(false, "application is running already"); return; }

	init();

	while (!custom::system::should_close) {
		if (!app.window) { CUSTOM_ASSERT(false, "application has no window"); break; }
		if (!custom::window::get_is_active(app.window)) { CUSTOM_ASSERT(false, "application window is inactive"); break; }

		//
		u64 time_system = custom::timer::get_ticks();
		consume_config();
		custom::window::update(app.window);
		custom::system::update();
		time_system = custom::timer::get_ticks() - time_system;

		//
		u16 refresh_rate = app.refresh_rate.as_display
			? custom::window::get_refresh_rate(app.window, app.refresh_rate.target)
			: app.refresh_rate.target;

		u64 time_frame = wait_till_next_frame(
			app.frame_start_ticks, refresh_rate, custom::window::check_vsync(app.window), app.sleep_while_waiting
		);
		app.frame_start_ticks = custom::timer::get_ticks();

		r32 dt = (r32)time_frame / custom::timer::ticks_per_second;
		if (dt > (r32)app.refresh_rate.debug / refresh_rate) { dt = 1.0f / refresh_rate; }
		else if (dt > (r32)app.refresh_rate.failsafe / refresh_rate) { dt = (r32)app.refresh_rate.failsafe / refresh_rate; }

		// process the frame
		u64 time_logic = custom::timer::get_ticks();
		CALL_SAFELY(app.callbacks.update, dt);
		time_logic = custom::timer::get_ticks() - time_logic;

		//
		u64 time_render = custom::timer::get_ticks();
		custom::graphics::consume(app.bytecode_loader);
		custom::graphics::consume(app.bytecode_renderer);
		app.bytecode_loader.reset();
		app.bytecode_renderer.reset();
		time_render = custom::timer::get_ticks() - time_render;

		//
		DISPLAY_PERFORMANCE(
			app.window,
			time_frame,
			time_system, time_logic, time_render,
			custom::timer::ticks_per_second, dt
		);

		if (custom::application::get_key_transition(custom::Key_Code::F5, true)) {
			custom::Asset::update();
		}

		if (app.update_assets_automatically) { custom::Asset::update(); }
	}

	custom::file::watch_shutdown();
	custom::timer::shutdown();
	custom::graphics::shutdown();
	if (app.window) { custom::window::destroy(app.window); }

	is_running = false;
}

void set_refresh_rate(u32 target, u32 debug, u32 failsafe, u32 vsync, bln as_display) {
	app.refresh_rate = {(u16)target, (u8)debug, (u8)failsafe, (u8)vsync, (b8)as_display};
	if (app.window) { custom::window::set_vsync(app.window, app.refresh_rate.vsync); }
}

void toggle_borderless_fullscreen(void) {
	custom::window::toggle_borderless_fullscreen(app.window);
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

void set_close_callback(close_func * callback) {
	app.callbacks.close = callback;
}

}}
