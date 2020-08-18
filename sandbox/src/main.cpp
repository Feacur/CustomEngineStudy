#include "custom_engine.h"

#include "engine/impl/math_linear.h"
#include "engine/impl/asset_system.h"

#include "asset_system/uniform_ids.h"
#include "entity_system/component_types.h"
#include "entity_system/ecs_renderer.h"
#include "entity_system/ecs_lua_runner.h"

#include "asset_system/asset_types.h"

#include <lua.hpp>

// studying these:
// https://github.com/etodd/lasercrabs
// https://github.com/Marzac/le3d

custom::Entity create_visual(Visual visual_data, Transform transform_data) {
	custom::Entity entity = custom::Entity::create();
	(*entity.add_component<Visual>().get_fast()) = visual_data;
	(*entity.add_component<Transform>().get_fast()) = transform_data;
	return entity;
}

static Transform camera_transform;
static Camera camera;
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

	L = luaL_newstate();
	init_client_loader(L);
	custom::lua::init_math_linear(L);
	custom::lua::init_asset_system(L);
	custom::lua::init_entity_system(L);

	// luaL_openlibs(lua);
	luaL_requiref(L, LUA_GNAME, luaopen_base, 1); lua_pop(L, 1);
	// luaL_requiref(L, LUA_STRLIBNAME, luaopen_string, 1); lua_pop(L, 1);

	// @Note: init data
	camera_transform = {
		{0, 2, -5}, {0, 0, 0, 1}, {1, 1, 1}
	};

	// @Note: init entities
	custom::Entity entity11 = custom::Entity::create();
	custom::Entity entity21 = custom::Entity::create();
	custom::Entity entity31 = custom::Entity::create();

	entity31.destroy();
	entity11.destroy();

	custom::Entity entity41 = custom::Entity::create();

	entity21.destroy();
	entity41.destroy();

	custom::Asset::add<Lua_Asset>("assets/scripts/main.lua");
	sandbox::ecs_lua_runner::lua_function(L, "global_init");
}

r32 camera_zoom = 1;
ivec2 viewport_size;
static void update_camera_projection(void) {
	r32 const near = 0.1f; r32 const far  = 20.0f;
	r32 const scale_x = camera_zoom / tangent((pi / 2) / 2);
	r32 const aspect = (r32)viewport_size.x / (r32)viewport_size.y;
	camera.projection = mat_persp({scale_x, scale_x * aspect}, near, far);
}

static void on_app_viewport(ivec2 size) {
	viewport_size = size;
	update_camera_projection();
	custom::renderer::viewport({0, 0}, size);
}

static void on_app_update(r32 dt) {
	vec2 wheel = custom::application::get_mouse_wheel();
	if (wheel.y != 0) {
		camera_zoom = clamp(camera_zoom + wheel.y * dt, 0.5f, 2.0f);
		update_camera_projection();
	}

	if (custom::application::get_mouse_key(custom::Mouse_Code::Key2)) {
		ivec2 mouse_delta = custom::application::get_mouse_delta();

		#define GET_KEY_HELD_IMPL(key) (custom::application::get_key(custom::Key_Code::key))
		#define GET_DIR_IMPL(pos, neg) (r32)(GET_KEY_HELD_IMPL(pos) - GET_KEY_HELD_IMPL(neg))
		vec3 move_delta{GET_DIR_IMPL(D, A), GET_DIR_IMPL(E, Q), GET_DIR_IMPL(W, S)};
		r32 move_speed = GET_KEY_HELD_IMPL(Shift) ? 4.0f : 1.0f;
		#undef GET_KEY_HELD_IMPL
		#undef GET_DIR_IMPL

		// @Note: object-space rotation
		camera_transform.rotation = normalize(quat_product(
			camera_transform.rotation,
			quat_from_radians(
				vec3{-(r32)mouse_delta.y, (r32)mouse_delta.x, 0} * (0.3f * dt)
			)
		));

		camera_transform.position += quat_rotate_vector(camera_transform.rotation, move_delta) * (move_speed * dt);
	}

	// render entities
	custom::renderer::clear();
	sandbox::ecs_renderer::update(camera_transform, camera.projection);
	sandbox::ecs_lua_runner::lua_function(L, "global_update");
	sandbox::ecs_lua_runner::update(L, dt);
}

int main(int argc, char * argv[]) {
	custom::application::set_init_callback(&on_app_init);
	custom::application::set_viewport_callback(&on_app_viewport);
	custom::application::set_update_callback(&on_app_update);
	custom::application::run();
	// getchar();
	return 0;
}
