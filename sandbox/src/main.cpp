#include "custom_engine.h"

#include "assets/ids.h"
#include "entity_system/components.h"
#include "entity_system/ecs_renderer.h"
#include "entity_system/ecs_lua_runner.h"

// studying these:
// https://github.com/etodd/lasercrabs
// https://github.com/Marzac/le3d

custom::Entity create_visual(custom::Bytecode * bc, Visual visual_data, Transform transform_data) {
	custom::Entity entity = custom::Entity::create();
	entity.add_component<Visual>(visual_data);
	entity.add_component<Transform>(transform_data);

	custom::loader::shader(bc, visual_data.shader);
	custom::loader::image(bc, visual_data.texture);
	custom::loader::mesh_obj(bc, visual_data.mesh);
	return entity;
}

Camera camera;
custom::Entity suzanne;

void init_entity_components(void);
static void on_app_init(custom::Bytecode * loader_bc, custom::Bytecode * renderer_bc) {
	init_entity_components();
	sandbox::ecs_renderer::init(renderer_bc);
	sandbox::ecs_lua_runner::init();

	camera.transform = {
		{0, 2, -5}, {0, 0, 0, 1}, {1, 1, 1}
	};

	//

	custom::Entity entity11 = custom::Entity::create();
	custom::Entity entity21 = custom::Entity::create();
	custom::Entity entity31 = custom::Entity::create();

	suzanne = create_visual(
		loader_bc,
		{
			(u32)sandbox::Shader::v3_texture_tint,
			(u32)sandbox::Texture::checkerboard,
			(u32)sandbox::Mesh::suzanne,
		},
		{{0, 1, 0}, {0, 0, 0, 1}, {1, 1, 1}}
	);

	create_visual(
		loader_bc,
		{
			(u32)sandbox::Shader::v3_texture_tint,
			(u32)sandbox::Texture::checkerboard,
			(u32)sandbox::Mesh::suzanne,
		},
		{{3, 2, 0}, {0, 0, 0, 1}, {1, 2, 1}}
	);

	create_visual(
		loader_bc,
		{
			(u32)sandbox::Shader::v3_texture_tint,
			(u32)sandbox::Texture::checkerboard,
			(u32)sandbox::Mesh::suzanne,
		},
		{{-5, 1, 0}, {0, 0, 0, 1}, {2, 1, 2}}
	);

	custom::Entity::destroy(entity21);
	custom::Entity::destroy(entity11);
	
	create_visual(
		loader_bc,
		{
			(u32)sandbox::Shader::v3_texture_tint,
			(u32)sandbox::Texture::proto_blue,
			(u32)sandbox::Mesh::plane_xz,
		},
		{{0, 0, 0}, {0, 0, 0, 1}, {10, 10, 10}}
	);

	custom::Entity::destroy(entity31);
	
	custom::Entity script_entity = custom::Entity::create();
	script_entity.add_component<Lua_Script>(Lua_Script{"assets/scripts/test.lua", false});
}

r32 camera_zoom = 1;
ivec2 viewport_size;
static void update_camera_projection(void) {
	r32 const near = 0.1f; r32 const far  = 20.0f;
	r32 const scale_x = camera_zoom / tangent((pi / 2) / 2);
	r32 const aspect = (r32)viewport_size.x / (r32)viewport_size.y;
	camera.projection = mat_persp01({scale_x, scale_x * aspect}, 0.1f, 20.0f);
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
		camera.transform.rotation = normalize(quat_product(
			camera.transform.rotation,
			quat_from_radians(
				vec3{-(r32)mouse_delta.y, (r32)mouse_delta.x, 0} * (0.3f * dt)
			)
		));

		camera.transform.position += quat_rotate_vector(camera.transform.rotation, move_delta) * (move_speed * dt);
	}

	static bool rotate_suzanne = false;
	if (custom::application::get_key_transition(custom::Key_Code::Space, true)) {
		rotate_suzanne = !rotate_suzanne;
	}
	if (rotate_suzanne) {
		// @Note: world-space rotation
		Transform * suzanne_transform = suzanne.get_component<Transform>().get_safe();
		suzanne_transform->rotation = normalize(quat_product(
			quat_from_radians(
				vec3{0.1f, 0.3f, 0.05f} * dt
			),
			suzanne_transform->rotation
		));
	}

	// render entities
	custom::renderer::clear();
	sandbox::ecs_renderer::process(camera.transform, camera.projection);
	sandbox::ecs_lua_runner::process();
}

int main(int argc, char * argv[]) {
	custom::application::set_init_callback(&on_app_init);
	custom::application::set_viewport_callback(&on_app_viewport);
	custom::application::set_update_callback(&on_app_update);
	custom::application::run();
	// getchar();
	return 0;
}
