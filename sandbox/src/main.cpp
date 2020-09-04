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
static custom::Asset_RefT<custom::Config_Asset> config_ref = {custom::empty_ref, custom::empty_index};

static void consume_config(void) {
	static u32 version = custom::empty_index;

	custom::Config_Asset const * config = config_ref.ref.get_safe();
	CUSTOM_ASSERT(config, "no config");

	if (version == config->version) { return; }
	version = config->version;

	u32 rr_target     = config->get_value<u32>("refresh_rate_target",     144);
	u32 rr_debug      = config->get_value<u32>("refresh_rate_debug",      20);
	u32 rr_failsafe   = config->get_value<u32>("refresh_rate_failsafe",   10);
	u32 rr_vsync      = config->get_value<u32>("refresh_rate_vsync",      1);
	bln rr_as_display = config->get_value<bln>("refresh_rate_as_display", true);
	custom::application::set_refresh_rate(
		rr_target, rr_debug, rr_failsafe, rr_vsync, rr_as_display
	);
}

void init_client_asset_types(void);
void init_client_component_types(void);
void init_uniform_names(void);
void init_client_loader(lua_State * L);

static void on_app_init() {
	// @Note: init systems
	init_client_asset_types();
	init_client_component_types();

	// @Note init configs
	u32 config_id = custom::Asset::store_string("assets/configs/client.cfg", custom::empty_index);
	config_ref = custom::Asset::add<custom::Config_Asset>(config_id);

	consume_config();

	custom::file::watch_init(".", true);

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

	u32 lua_id = custom::Asset::store_string("assets/scripts/main.lua", custom::empty_index);
	custom::Asset::add<Lua_Asset>(lua_id);

	//
	sandbox::ecs_init_physics();

	// @Note: call Lua init
	sandbox::lua_function(L, "global_init");
}

static void on_app_viewport(ivec2 size) {
	custom::renderer::viewport({0, 0}, size);
}

static void on_app_update(r32 dt) {
	consume_config();
	sandbox::lua_function(L, "global_update");
	sandbox::ecs_update_lua(L, dt);
	sandbox::ecs_update_physics(dt);
	sandbox::ecs_update_renderer();
}

int main(int argc, char * argv[]) {
	CUSTOM_TRACE("__cplusplus: %d", __cplusplus);
	custom::application::set_init_callback(&on_app_init);
	custom::application::set_viewport_callback(&on_app_viewport);
	custom::application::set_update_callback(&on_app_update);
	custom::application::init();

	custom::application::run();
	custom::file::watch_shutdown();
	// getchar();
	return 0;
}
