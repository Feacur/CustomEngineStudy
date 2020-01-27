#include "custom_engine.h"

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

static void reset_graphics_settings(custom::Command_Buffer & gvm_buffer) {
	gvm_buffer.write(custom::Graphics_Instruction::Depth_Read);
	gvm_buffer.write((u8)1);

	gvm_buffer.write(custom::Graphics_Instruction::Depth_Write);
	gvm_buffer.write((u8)1);

	gvm_buffer.write(custom::Graphics_Instruction::Depth_Comparison);
	gvm_buffer.write(custom::Comparison::Less);

	gvm_buffer.write(custom::Graphics_Instruction::Blend_Mode);
	gvm_buffer.write(custom::Blend_Mode::Alpha);

	gvm_buffer.write(custom::Graphics_Instruction::Cull_Mode);
	gvm_buffer.write(custom::Cull_Mode::Back);
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

	custom::Graphics_VM gvm;

	custom::Command_Buffer gvm_buffer;
	reset_graphics_settings(gvm_buffer);

	custom::load_image(gvm_buffer, 0, "assets/textures/checkerboard.png");
	gvm_buffer.write(custom::Graphics_Instruction::Free_Texture);
	gvm_buffer.write((u32)0);

	ivec2 viewport_position = {};
	ivec2 viewport_size = window->get_size();
	gvm_buffer.write(custom::Graphics_Instruction::Viewport);
	gvm_buffer.write(viewport_position);
	gvm_buffer.write(viewport_size);

	while (true) {
		if (custom::system.should_close) { break; }
		if (custom::Window::should_close) { break; }

		// prepare for a frame
		u64 last_frame_ticks = get_last_frame_ticks(window->is_vsync());
		DISPLAY_PERFORMANCE(*window, last_frame_ticks, custom::timer.ticks_per_second);

		// process the frame
		gvm_buffer.write(custom::Graphics_Instruction::Clear);
		gvm_buffer.write(custom::Clear_Flags::Color | custom::Clear_Flags::Depth);

		custom::system_update();
		gvm.render(gvm_buffer);
		window->update();

		// clean up after the frame
		gvm_buffer.offset = 0;
		gvm_buffer.bytecode.count = 0;
	}

	delete window;

	// getchar();
	return 0;
}
