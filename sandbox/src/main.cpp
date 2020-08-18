#include "custom_engine.h"

#include "engine/impl/asset_system.h"

#include "asset_system/uniform_ids.h"
#include "asset_system/asset_types.h"

#include "entity_system/ecs_renderer.h"
#include "entity_system/ecs_lua_runner.h"

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
	init_client_asset_types();
	init_client_component_types();

	init_uniform_names();
	custom::loader::uniforms();

	// @Note: init Lua
	L = luaL_newstate();
	init_client_loader(L);
	custom::lua::init_input(L);
	custom::lua::init_math_linear(L);
	custom::lua::init_asset_system(L);
	custom::lua::init_entity_system(L);

	// luaL_openlibs(lua);
	luaL_requiref(L, LUA_GNAME, luaopen_base, 1); lua_pop(L, 1);
	luaL_requiref(L, LUA_MATHLIBNAME, luaopen_math, 1); lua_pop(L, 1);
	// luaL_requiref(L, LUA_STRLIBNAME, luaopen_string, 1); lua_pop(L, 1);

	custom::Asset::add<Lua_Asset>("assets/scripts/main.lua");

	// @Note: call Lua init
	sandbox::ecs_lua_runner::lua_function(L, "global_init");
}

static void on_app_viewport(ivec2 size) {
	custom::renderer::viewport({0, 0}, size);
}

static void on_app_update(r32 dt) {
	sandbox::ecs_lua_runner::lua_function(L, "global_update");
	sandbox::ecs_lua_runner::update(L, dt);

	custom::renderer::clear();
	sandbox::ecs_renderer::update();
}

int main(int argc, char * argv[]) {
	custom::application::set_init_callback(&on_app_init);
	custom::application::set_viewport_callback(&on_app_viewport);
	custom::application::set_update_callback(&on_app_update);
	custom::application::run();
	// getchar();
	return 0;
}
