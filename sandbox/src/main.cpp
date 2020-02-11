#include "custom_engine.h"
#include "assets/ids.h"
#include "entity_system/components.h"
#include "entity_system/renderer.h"

// studying these:
// https://github.com/etodd/lasercrabs
// https://github.com/Marzac/le3d

#if !defined(CUSTOM_SHIPPING)
static void display_performace(custom::Window & window, u64 duration, u64 precision) {
	float debug_ms = duration * 1000 / (float)precision;
	float debug_fps = precision / (float)duration;
	static char header_text[64];
	sprintf(header_text, "custom engine - %.1f ms (%.1f FPS)", debug_ms, debug_fps);
	window.set_header(header_text);
}
	#define DISPLAY_PERFORMANCE(window, duration, precision)\
		display_performace(window, duration, precision)
#else
	#define DISPLAY_PERFORMANCE(window, duration, precision)
#endif

static u64 get_last_frame_ticks(bool is_vsync) {
	static u64 const duration  = 16666;
	static u64 const precision = custom::microsecond;
	if (is_vsync) {
		return custom::timer_snapshot();
	}
	return custom::timer_wait_next_frame(duration, precision);
}

static u32 create_quads_3_4(u32 local_id, u32 capacity) {
	using namespace custom::graphics;
	custom::Array_Fixed<custom::runtime::Buffer, 2> buffers;
	buffers.push({
		false, Mesh_Frequency::Dynamic, Mesh_Access::Draw,
		Data_Type::r32, (3 + 4) * 4 * capacity, 0,
		2, {3, 4}
	});
	buffers.push({
		true, Mesh_Frequency::Static, Mesh_Access::Draw,
		Data_Type::u16, 3 * 2 * capacity, 0,
		0, {}
	});
	return custom::loader::create_mesh(local_id, buffers.data, (u8)buffers.count);
}

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

int main(int argc, char * argv[]) {
	// @Note: use structs and global functions; there is no need in RAII here
	//        or resources management in the first place whatsoever.
	//        System and Timer are global anyway; why pretending otherwise?
	//        Window doesn't manage its resources and can get a "destroy" message!
	custom::system_init();
	custom::timer_init();
	// @Note: on the other hand, automatic call of a constructor for a global instance
	//        is cool, right? not much beneficial, and potentially malicious, though.

	// @Note: it's silly to init a context and then manually create a renderer.
	//        probably it's better to construct a renderer with
	custom::Window * window = new custom::Window;
	window->init_context();
	window->set_vsync(1);

	custom::graphics::VM gvm;

	custom::Bytecode gbc;
	custom::loader::init(&gbc);
	custom::renderer::init(&gbc);
	sandbox::renderer::init(&gbc);

	ivec2 size = window->get_size();
	r32 const scale = 1 / tangent((pi / 2) / 2);
	r32 const aspect = (r32)size.y / (r32)size.x;
	custom::renderer::viewport({0, 0}, size);
	mat4 cam = mat_product(
		mat_inverse_transform(mat_position_scale({0, 0, 0}, {1, 1, 1})),
		mat_persp({scale * aspect, scale}, 0.1f, 10.0f)
		// mat_ortho({scale * aspect, scale}, 0, 10)
	);

	custom::Entity::component_types_count = 2;
	Visual::offset = 0;
	Transform::offset = 1;
	custom::World::component_pools.push(&Visual::pool);
	custom::World::component_pools.push(&Transform::pool);

	u32 quad_asset_id = custom::loader::create_quad((u32)sandbox::Runtime_Mesh::quad);
	u32 cube_asset_id = custom::loader::create_cube((u32)sandbox::Runtime_Mesh::cube);
	u32 particle_test_asset_id = create_quads_3_4(
		(u32)sandbox::Runtime_Mesh::particle_test, 128
	);

	custom::Ref<custom::Entity> entity1 = create_visual(
		(u32)sandbox::Shader::renderer2d,
		(u32)sandbox::Texture::checkerboard,
		quad_asset_id,
		{1.0f, 0.5f, 1.2f}
	);
	custom::Ref<custom::Entity> entity2 = create_visual(
		empty_id, empty_id, empty_id,
		{0, 0, 0}
	);
	custom::Ref<custom::Entity> entity3 = create_visual(
		(u32)sandbox::Shader::renderer2d,
		empty_id,
		particle_test_asset_id,
		{0, 0, 0}
	);
	custom::Ref<custom::Entity> entity4 = create_visual(
		(u32)sandbox::Shader::renderer3d,
		(u32)sandbox::Texture::checkerboard,
		cube_asset_id,
		{-0.5f, 0, 1.5f}
	);

	custom::World::destroy(entity2);

	while (true) {
		if (custom::system.should_close) { break; }
		if (custom::Window::should_close) { break; }

		// prepare for a frame
		u64 last_frame_ticks = get_last_frame_ticks(window->is_vsync());
		DISPLAY_PERFORMANCE(*window, last_frame_ticks, custom::timer.ticks_per_second);

		r32 dt = (r32)last_frame_ticks / custom::timer.ticks_per_second;

		// process the frame
		custom::system_update();

		// @Todo: input, logic
		Transform * transform4 = entity4->get_component<Transform>().operator->();
		if (transform4) {
			transform4->rotation = normalize(quat_product(
				transform4->rotation,
				quat_from_radians({pi * 0.05f * dt, pi * 0.2f * dt, pi * 0.1f * dt})
			));
		}

		// render entities
		custom::renderer::clear();
		sandbox::renderer::update(cam);

		gvm.update(gbc);
		window->update();

		// clean up after the frame
		gbc.offset = 0;
		gbc.buffer.count = 0;
	}

	delete window;

	// getchar();
	return 0;
}
