#include "main_common.h"
#include "demo_game/platform_globals.h"

#include "data.h"
#include "code.h"

API_C API_DLL GAME_UPDATE(game_update) {
	globals::cache(platform_data);

	auto game_data = get_game_data();

	// request render buffer size
	platform_data->render_settings.target_size = {320, 180};
	platform_data->render_settings.size_mode = Render_Settings::Size_Mode::Game;
	platform_data->render_settings.stretch_mode = Render_Settings::Stretch_Mode::Fractional;
}

API_C API_DLL GAME_RENDER(game_render) {
	clear_buffer(globals::render_buffer, {0, 0, 0, 0});

	auto game_data = get_game_data();
}

API_C API_DLL GAME_OUTPUT_SOUND(game_output_sound) {
	auto game_data = get_game_data();
	
	static float sample_time = 0;
	static float volume = 5000;

	auto sample_step = tau / (platform_data->sound_buffer_sound.samples_per_second / 400.f);
	
	int16 *sample_out = platform_data->sound_buffer_sound.data;
	for (int32 block_index = 0; block_index < samples_count; ++block_index) {
		sample_time += sample_step;
		if (sample_time > tau) { sample_time = sample_time - tau; }
		int16 sample_value = (int16)(sinf(sample_time) * volume);
		for (int32 channel_index = 0; channel_index < platform_data->sound_buffer_sound.channels; ++channel_index) {
			*sample_out++ = sample_value;
		}
	}
}
