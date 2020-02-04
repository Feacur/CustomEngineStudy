#include "custom_engine.h"
#include "assets/ids.h"

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

namespace custom {
namespace graphics {

static void reset_settings(Bytecode & bc) {
	bc.write(Instruction::Message_Inline);
	bc.write("reset settings");

	bc.write(Instruction::Depth_Read);
	bc.write((b8)1);

	bc.write(Instruction::Depth_Write);
	bc.write((b8)1);

	bc.write(Instruction::Depth_Comparison);
	bc.write(Comparison::Less);

	bc.write(Instruction::Blend_Mode);
	bc.write(Blend_Mode::Alpha);

	bc.write(Instruction::Cull_Mode);
	bc.write(Cull_Mode::Back);
}

static void clear(Bytecode & bc) {
	bc.write(Instruction::Clear);
	bc.write(Clear_Flags::Color | Clear_Flags::Depth);
}

}}

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
	custom::graphics::reset_settings(gbc);

	custom::init_uniforms(gbc);
	custom::load_shader(gbc, (u32)sandbox::Shader::device);
	custom::load_shader(gbc, (u32)sandbox::Shader::particle_device);
	custom::load_image(gbc, (u32)sandbox::Texture::checkerboard);
	custom::load_quad(gbc, (u32)sandbox::Mesh::quad);
	custom::load_particle_test(gbc, (u32)sandbox::Mesh::particle_test);

	ivec2 viewport_position = {};
	ivec2 viewport_size = window->get_size();
	gbc.write(custom::graphics::Instruction::Viewport);
	gbc.write(viewport_position);
	gbc.write(viewport_size);

	while (true) {
		if (custom::system.should_close) { break; }
		if (custom::Window::should_close) { break; }

		// prepare for a frame
		u64 last_frame_ticks = get_last_frame_ticks(window->is_vsync());
		DISPLAY_PERFORMANCE(*window, last_frame_ticks, custom::timer.ticks_per_second);

		// process the frame
		custom::system_update();
		// @Todo: input, logic

		custom::graphics::clear(gbc);

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
				gbc.write(i);
			}

			for (u32 i = 0; i < slots_count; ++i) {
				gbc.write(custom::graphics::Instruction::Load_Uniform);
				gbc.write(sandbox::Shader::device);
				gbc.write(texture_uniforms[i]);
				gbc.write(custom::graphics::Data_Type::sampler_unit);
				gbc.write((u32)1); gbc.write(custom::graphics::sampler_unit { i });
			}

			gbc.write(custom::graphics::Instruction::Use_Mesh);
			gbc.write(sandbox::Mesh::quad);

			gbc.write(custom::graphics::Instruction::Draw);
		}

		// particle_test
		{
			gbc.write(custom::graphics::Instruction::Use_Shader);
			gbc.write(sandbox::Shader::particle_device);

			gbc.write(custom::graphics::Instruction::Use_Mesh);
			gbc.write(sandbox::Mesh::particle_test);

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
