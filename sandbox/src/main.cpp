#include "custom_engine.h"

#include "engine/impl/asset_system.h"

#include "asset_system/asset_types.h"

#include "entity_system/ecs_lua_runner.h"
#include "entity_system/ecs_physics.h"
#include "entity_system/ecs_renderer.h"

#include <lua.hpp>

// studying these:
// https://github.com/etodd/lasercrabs
// https://github.com/Marzac/le3d

static lua_State * L;

void init_client_asset_types(void);
void init_client_component_types(void);
void init_uniform_names(void);
void init_client_loader(lua_State * L);

static void on_app_init() {
	// @Note: init systems
	custom::file::watch_init(".", true);
	init_client_asset_types();
	init_client_component_types();

	// @Note: init Lua
	L = luaL_newstate();
	init_client_loader(L);
	custom::lua::init_input(L);
	custom::lua::init_graphics_params(L);
	custom::lua::init_math_linear(L);
	custom::lua::init_asset_system(L);
	custom::lua::init_entity_system(L);

	// luaL_openlibs(lua);
	luaL_requiref(L, LUA_GNAME, luaopen_base, 1); lua_pop(L, 1);
	luaL_requiref(L, LUA_MATHLIBNAME, luaopen_math, 1); lua_pop(L, 1);
	// luaL_requiref(L, LUA_STRLIBNAME, luaopen_string, 1); lua_pop(L, 1);

	u32 id = custom::Asset::store_string("assets/scripts/main.lua", custom::empty_index);
	custom::Asset::add<Lua_Asset>(id);

	// @Note: call Lua init
	sandbox::lua_function(L, "global_init");
}

static void on_app_viewport(ivec2 size) {
	custom::renderer::viewport({0, 0}, size);
}

static void on_app_update(r32 dt) {
	custom::file::watch_update();
	for (u32 i = 0; i < custom::file::actions.count; ++i) {
		switch (custom::file::actions[i].type) {
			case custom::file::Action_Type::Add: CUSTOM_TRACE("add: '%s'", custom::file::strings.get_string(custom::file::actions[i].id)); break;
			case custom::file::Action_Type::Rem: CUSTOM_TRACE("rem: '%s'", custom::file::strings.get_string(custom::file::actions[i].id)); break;
			case custom::file::Action_Type::Mod: CUSTOM_TRACE("mod: '%s'", custom::file::strings.get_string(custom::file::actions[i].id)); break;
			case custom::file::Action_Type::Old: CUSTOM_TRACE("old: '%s'", custom::file::strings.get_string(custom::file::actions[i].id)); break;
			case custom::file::Action_Type::New: CUSTOM_TRACE("new: '%s'", custom::file::strings.get_string(custom::file::actions[i].id)); break;
		}
	}
	sandbox::lua_function(L, "global_update");
	sandbox::ecs_update_lua(L, dt);
	sandbox::ecs_update_physics(dt);
	sandbox::ecs_update_renderer();
}

static void hint_graphics(void) {
	// context_settings
	// custom::context_settings = {};
	// custom::context_settings.major_version = 4;
	// custom::context_settings.minor_version = 6;

	// pixel_format_hint
	// custom::pixel_format_hint = {};
	// custom::pixel_format_hint.red_bits     = 8;
	// custom::pixel_format_hint.green_bits   = 8;
	// custom::pixel_format_hint.blue_bits    = 8;
	// custom::pixel_format_hint.alpha_bits   = 8;
	// custom::pixel_format_hint.depth_bits   = 24;
	// custom::pixel_format_hint.stencil_bits = 8;
	// custom::pixel_format_hint.doublebuffer = true;
	// custom::pixel_format_hint.swap         = 1;
}

int main(int argc, char * argv[]) {
	CUSTOM_TRACE("__cplusplus: %d", __cplusplus);

	hint_graphics();
	custom::application::set_refresh_rate(60, 10, 1, false);
	custom::application::set_init_callback(&on_app_init);
	custom::application::set_viewport_callback(&on_app_viewport);
	custom::application::set_update_callback(&on_app_update);
	custom::application::run();
	custom::file::watch_shutdown();
	// getchar();
	return 0;
}
