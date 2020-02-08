#include "custom_engine.h"
#include "assets/ids.h"
#include "entity_system.h"

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
		Data_Type::r32, (3 + 4) * 4 * capacity, 0, {}
	});
	buffers[0].attributes.push(3); buffers[0].attributes.push(4);
	buffers.push({
		true, Mesh_Frequency::Static, Mesh_Access::Draw,
		Data_Type::u16, 3 * 2 * capacity, 0, {}
	});
	return custom::loader::create_mesh(local_id, buffers.data, (u8)buffers.count);
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

	custom::loader::shader((u32)sandbox::Shader::device);
	custom::loader::shader((u32)sandbox::Shader::particle_device);
	custom::loader::image((u32)sandbox::Texture::checkerboard);

	u32 quad_asset_id = custom::loader::create_quad((u32)sandbox::Runtime_Mesh::quad);
	u32 particle_test_asset_id = create_quads_3_4(
		(u32)sandbox::Runtime_Mesh::particle_test, 128
	);

	custom::renderer::viewport({0, 0}, window->get_size());

	custom::Entity::component_types_count = 1;
	custom::World::component_pools.push(&Visual::pool);

	custom::Ref<custom::Entity> entity1 = custom::World::create();
	custom::Ref<custom::Entity> entity2 = custom::World::create();
	custom::Ref<custom::Entity> entity3 = custom::World::create();

	entity1->add_component<Visual>();
	entity2->add_component<Visual>();
	entity3->add_component<Visual>();

	custom::Entity::pool.destroy(entity2);

	while (true) {
		if (custom::system.should_close) { break; }
		if (custom::Window::should_close) { break; }

		// prepare for a frame
		u64 last_frame_ticks = get_last_frame_ticks(window->is_vsync());
		DISPLAY_PERFORMANCE(*window, last_frame_ticks, custom::timer.ticks_per_second);

		// process the frame
		custom::system_update();
		// @Todo: input, logic

		custom::renderer::clear();

		// @Note: thoughts on factoring code for a renderer
		//        - batch together materials/objects with the same shaders
		//        - proactively bind textures pending to rendering
		//        - batch and rebind if running out of slots/units

		// quad
		{
			u32 const slots_count = 1;
			sandbox::Uniform texture_uniforms[slots_count] = { sandbox::Uniform::texture };
			sandbox::Texture texture_assets[slots_count] = { sandbox::Texture::checkerboard };

			gbc.write(custom::graphics::Instruction::Use_Shader);
			gbc.write(sandbox::Shader::device);

			for (u32 i = 0; i < slots_count; ++i) {
				gbc.write(custom::graphics::Instruction::Use_Texture);
				gbc.write(texture_assets[i]);
			}

			for (u32 i = 0; i < slots_count; ++i) {
				gbc.write(custom::graphics::Instruction::Load_Uniform);
				gbc.write(sandbox::Shader::device);
				gbc.write(texture_uniforms[i]);
				gbc.write(custom::graphics::Data_Type::texture_unit);
				gbc.write((u32)1); gbc.write(texture_assets[i]);
			}

			gbc.write(custom::graphics::Instruction::Use_Mesh);
			gbc.write(quad_asset_id);

			gbc.write(custom::graphics::Instruction::Draw);
		}

		// particle_test
		{
			gbc.write(custom::graphics::Instruction::Use_Shader);
			gbc.write(sandbox::Shader::particle_device);

			gbc.write(custom::graphics::Instruction::Use_Mesh);
			gbc.write(particle_test_asset_id);

			gbc.write(custom::graphics::Instruction::Draw);
		}

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
