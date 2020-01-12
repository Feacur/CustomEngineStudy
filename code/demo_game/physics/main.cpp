#include "demo_game/main_common.h"
#include "demo_game/platform_globals.h"

#include "shared/software_renderer_rect.h"
#include "shared/software_renderer_simd.h"

#include "data.h"
#include "code.h"

extern "C" CUSTOM_DLL GAME_UPDATE(game_update) {
	globals::cache(platform_data);

	auto game_data = get_game_data();
	
	game::update(game_data);
	
	// request render settings
	platform_data->render_settings.target_size = {960, 540};
	platform_data->render_settings.size_mode = Render_Settings::Size_Mode::Window;
	platform_data->render_settings.stretch_mode = Render_Settings::Stretch_Mode::None;
}

extern "C" CUSTOM_DLL GAME_RENDER(game_render) {
	clear_buffer(globals::render_buffer, {0, 0, 0, 0});

	auto game_data = get_game_data();

	Vector2 CAMERA_OFFSET = {
		globals::render_buffer.size.x * 0.5f,
		globals::render_buffer.size.y * 0.5f
	};
	
	// draw entities
	for (u8 i = 0; i < game_data->entities.length; ++i) {
		Entity entity = game_data->entities[i];
		
		Vector2 axis_x = entity.rotation;
		Vector2 axis_y = vec2(-axis_x.y, axis_x.x);
		Vector2 centre_offset = -(axis_x * entity.size.x + axis_y * entity.size.y) / 2;

		draw_rectangle(
			globals::render_buffer,
			CAMERA_OFFSET + entity.position + centre_offset,
			entity.size,
			entity.rotation,
			ENTITY_COLOR
		);
	}
}

// extern "C" CUSTOM_DLL GAME_OUTPUT_SOUND(game_output_sound) {
// 	auto game_data = get_game_data();
// }
