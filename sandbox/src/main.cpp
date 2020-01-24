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

	cstring msg_ptr1 = "hello, pointer 1!";
	cstring msg_ptr2 = "hello, pointer 2!";
	cstring msg = "hello, world!";
	u32 msg_len_full = (u32)strlen(msg) + 1;

	while (true) {
		if (custom::system.should_close) { break; }
		if (custom::Window::should_close) { break; }

		gvm_buffer.offset = 0;
		gvm_buffer.bytecode.count = 0;

		gvm_buffer.write(custom::Graphics_Instruction::Print_Pointer);
		gvm_buffer.write(msg_ptr1);

		gvm_buffer.write(custom::Graphics_Instruction::Print_Inline);
		gvm_buffer.write(msg_len_full);
		gvm_buffer.write(msg, msg_len_full);

		gvm_buffer.write(custom::Graphics_Instruction::Print_Pointer);
		gvm_buffer.write(msg_ptr2);

		u64 last_frame_ticks = get_last_frame_ticks(window->is_vsync());
		DISPLAY_PERFORMANCE(*window, last_frame_ticks, custom::timer.ticks_per_second);
		custom::system_update();
		gvm.render(gvm_buffer);
		window->update();
	}

	delete window;

	// getchar();
	return 0;
}
