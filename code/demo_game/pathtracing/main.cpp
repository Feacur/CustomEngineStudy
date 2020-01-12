#include "demo_game/main_common.h"
#include "demo_game/platform_globals.h"

#include "shared/math_shapes.h"

#include "data.h"
#include "code.h"

#include "scene_4.h"
#include "scene_common.h"

#include "routines_texture.h"
#include "routines_material.h"
#include "routines_camera.h"
#include "routines_shape.h"
#include "routines_scene.h"
#include "routines_draw.h"

// https://raytracing.github.io/

extern "C" CUSTOM_DLL GAME_UPDATE(game_update) {
	globals::cache(platform_data);

	auto game_data = get_game_data();

	camera::process_input_rotation(game_data);
	camera::process_input_position(game_data);
	
	// request render buffer size
	platform_data->render_settings.target_size = {320, 180};
	platform_data->render_settings.size_mode = Render_Settings::Size_Mode::Game;
	platform_data->render_settings.stretch_mode = Render_Settings::Stretch_Mode::Fractional;
}

extern "C" CUSTOM_DLL GAME_RENDER(game_render) {
	auto game_data = get_game_data();
	camera::reset_exposure(game_data);
	draw_hit_shapes(game_data, game_data->camera_position, game_data->camera_rotation);
	platform_data->render_buffer_image_f.exposure = globals::render_buffer_f.exposure;
}

// extern "C" CUSTOM_DLL GAME_OUTPUT_SOUND(game_output_sound) {
// 	auto game_data = get_game_data();
// }