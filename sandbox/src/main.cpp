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

int main(int argc, char * argv[]) {
	static u64 const duration  = 16666;
	static u64 const precision = 1000000;

	custom::System & system = custom::System::get();
	custom::Timer  & timer = custom::Timer::get();

	custom::Window window;
	window.init_context();

	custom::Opengl_Renderer renderer;

	static u64 const ticks_per_second = timer.get_ticks_per_second();
	while (true) {
		if (system.should_close) { break; }
		if (window.should_close) { break; }

		u64 last_frame_ticks = timer.wait_next_frame(duration, precision);
		DISPLAY_PERFORMANCE(window, last_frame_ticks, ticks_per_second);
		system.update();
		renderer.update();
		window.update();
	}

	return 0;
}
