#include "custom_engine.h"

#include "engine/impl/math_linear.h"
#include "engine/impl/asset_system.h"

#include "assets/ids.h"
#include "components/types.h"
#include "entity_system/ecs_renderer.h"
#include "entity_system/ecs_lua_runner.h"

#include <lua.hpp>
// #include <lstate.h>

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

static void init_client_assets(void) {
	// @Note: shaders
	(*custom::Asset_System::add_asset<custom::ShaderAsset>("assets/shaders/v2_texture_tint.glsl").get_fast()) = {
		custom::graphics::Shader_Part::Vertex | custom::graphics::Shader_Part::Pixel,
	};

	(*custom::Asset_System::add_asset<custom::ShaderAsset>("assets/shaders/v3_texture_tint.glsl").get_fast()) = {
		custom::graphics::Shader_Part::Vertex | custom::graphics::Shader_Part::Pixel,
	};

	// @Note: textures
	// "assets/textures/checkerboard.png",
	// "assets/textures/blue_noise.png",
	// "assets/textures/proto_blue.png",

	// @Note: meshes
	// "assets/meshes/plane_xz.obj",
	// "assets/meshes/suzanne.obj",
}

void init_asset_system(void);
void init_entity_components(void);
static void on_app_init() {
	// @Note: init systems
	init_asset_system();
	init_entity_components();

	init_client_assets();

	L = luaL_newstate();
	// luaL_openlibs(lua);
	luaL_requiref(L, LUA_GNAME, luaopen_base, 1); lua_pop(L, 1);
	luaL_requiref(L, LUA_STRLIBNAME, luaopen_string, 1); lua_pop(L, 1);
	custom::lua::init_math_linear(L);
	custom::lua::init_entity_system(L);

	for (u32 asset_id = 0; asset_id < (u32)sandbox::Script::count; ++asset_id) {
		custom::loader::script(L, asset_id);
	}

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
	
	create_visual(
		{
			custom::Asset_System::get_asset<custom::ShaderAsset>("assets/shaders/v3_texture_tint.glsl"),
			(u32)sandbox::Texture::proto_blue,
			(u32)sandbox::Mesh::plane_xz,
		},
		{{0, 0, 0}, {0, 0, 0, 1}, {10, 10, 10}}
	);

	entity21.destroy();

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
