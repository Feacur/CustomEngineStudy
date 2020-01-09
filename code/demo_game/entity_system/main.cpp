#include "demo_game/main_common.h"
#include "demo_game/platform_globals.h"

#include "shared/software_renderer_rect.h"
#include "shared/software_renderer_simd.h"

#include "shared/math_shapes.h"

#include "data.h"
#include "code.h"

// https://github.com/aras-p/dod-playground

API_C API_DLL GAME_UPDATE(game_update) {
	globals::cache(platform_data);
	
	auto game_data = get_game_data();
	
	game::update(game_data);
	
	// request render buffer size
	platform_data->render_settings.target_size = {960, 540};
	platform_data->render_settings.size_mode = Render_Settings::Size_Mode::Window;
	platform_data->render_settings.stretch_mode = Render_Settings::Stretch_Mode::None;
}

API_C API_DLL GAME_RENDER(game_render) {
	clear_buffer(globals::render_buffer, {0.1f, 0.1f, 0.1f, 1});

	auto game_data = get_game_data();
	
	Array_Dynamic<Renderable> renderables = game_data->renderables;
	for (size_t i = 0; i < renderables.length; ++i) {
		Renderable const & renderable = renderables[i];
		draw_rectangle(
			globals::render_buffer,
			renderable.position,
			renderable.size,
			renderable.rotation,
			renderable.color
		);
	}
}

// API_C API_DLL GAME_OUTPUT_SOUND(game_output_sound) {
// 	auto game_data = get_game_data();
// }
