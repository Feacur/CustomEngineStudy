#include "custom_engine.h"
#include "assets/ids.h"
#include "entity_system/components.h"
#include "entity_system/renderer.h"
#include "application.h"

// studying these:
// https://github.com/etodd/lasercrabs
// https://github.com/Marzac/le3d

// static u32 create_quads_3_4(u32 local_id, u32 capacity) {
// 	using namespace custom::graphics;
// 	custom::Array_Fixed<custom::runtime::Buffer, 2> buffers;
// 	buffers.push({
// 		false, Mesh_Frequency::Dynamic, Mesh_Access::Draw,
// 		Data_Type::r32, (3 + 4) * 4 * capacity, 0,
// 		2, {3, 4}
// 	});
// 	buffers.push({
// 		true, Mesh_Frequency::Static, Mesh_Access::Draw,
// 		Data_Type::u16, 3 * 2 * capacity, 0,
// 		0, {}
// 	});
// 	return custom::loader::create_mesh(local_id, buffers.data, (u8)buffers.count);
// }

custom::Ref<custom::Entity> create_visual(u32 shader, u32 texture, u32 mesh, vec3 position) {
	custom::Ref<custom::Entity> entity = custom::World::create();

	entity->add_component<Visual>();
	custom::Ref<Visual> visual = entity->get_component<Visual>();
	Visual * visual_ptr = visual.operator->();
	visual_ptr->shader  = shader;
	visual_ptr->texture = texture;
	visual_ptr->mesh    = mesh;

	entity->add_component<Transform>();
	custom::Ref<Transform> transform = entity->get_component<Transform>();
	Transform * transform_ptr = transform.operator->();
	transform_ptr->position = position;
	transform_ptr->rotation = quat_from_radians({0, 0, 0});
	transform_ptr->scale = {1, 1, 1};

	if (shader < (u32)sandbox::Shader::count) {
		custom::loader::shader(shader);
	}

	if (texture < (u32)sandbox::Texture::count) {
		custom::loader::image(texture);
	}

	if (mesh < (u32)sandbox::Mesh::count) {
		custom::loader::mesh(mesh);
	}

	return entity;
}

custom::Ref<custom::Entity> entity4;
void init_entity_components(void);
static void impl_init() {
	init_entity_components();

	u32 cube_asset_id = custom::loader::create_cube((u32)sandbox::Runtime_Mesh::cube);
	entity4 = create_visual(
		(u32)sandbox::Shader::v3_texture_tint,
		(u32)sandbox::Texture::checkerboard,
		cube_asset_id,
		{0.0f, 0.0f, 1.5f}
	);
}

struct Camera
{
	Transform transform;
	mat4 projection;
};

struct Camera2d
{
	Transform2d transform;
	mat3 projection;
};

Camera camera;
r32 camera_zoom = 1;
ivec2 camera_size;
static void update_camera() {
	r32 const scale = camera_zoom / tangent((pi / 2) / 2);
	r32 const aspect = (r32)camera_size.y / (r32)camera_size.x;
	camera = {
		{{0, 0, 0}, quat_from_radians({0, 0, 0}), {1, 1, 1}},
		mat_persp({scale * aspect, scale}, 0.1f, 10.0f)
		// mat_ortho({scale * aspect, scale}, 0, 10)
	};
	// Camera2d camera2d = {
	// 	{{0, 0}, complex_from_radians(0), {1, 1}},
	// 	mat_position_scale(vec2{0, 0}, vec2{scale * aspect, scale})
	// };
}

static void impl_viewport(ivec2 size) {
	camera_size = size;
	update_camera();
}

static void impl_update(r32 dt) {
	vec2 wheel = sandbox::application::get_mouse_wheel();
	if (wheel.y != 0.0f) {
		camera_zoom = clamp(camera_zoom + wheel.y * dt, 0.5f, 2.0f);
		update_camera();
	}

	vec3 delta_euler_angles;
	if (sandbox::application::get_mouse_key(custom::Mouse_Code::Key1) != custom::Key_State::Released) {
		ivec2 mouse_delta = sandbox::application::get_mouse_delta();
		delta_euler_angles.x = mouse_delta.y * 0.1f;
		delta_euler_angles.y = -mouse_delta.x * 0.1f;
		delta_euler_angles.z = 0;
	}
	else {
		delta_euler_angles = {0.05f, 0.2f, 0.1f};
	}

	Transform * transform4 = entity4->get_component<Transform>().operator->();
	if (transform4) {
		// @Note: rotate in world space
		transform4->rotation = normalize(quat_product(
			quat_from_radians(delta_euler_angles * (pi * dt)), transform4->rotation
		));
	}

	// render entities
	custom::renderer::clear();
	sandbox::renderer::update(camera.transform, camera.projection);
}

int main(int argc, char * argv[]) {
	sandbox::application::set_init_callback(&impl_init);
	sandbox::application::set_viewport_callback(&impl_viewport);
	sandbox::application::set_update_callback(&impl_update);
	sandbox::application::run();
	// getchar();
	return 0;
}
