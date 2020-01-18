#include "demo_game/main_common.h"
#include "demo_game/platform_globals.h"

#include "shared/software_renderer_rect.h"
#include "shared/software_renderer_simd.h"

#include "data.h"
#include "data_prefabs.h"
#include "code.h"

extern "C" CUSTOM_DLL GAME_UPDATE(game_update) {
	globals::cache(platform_data);

	auto game_data = get_game_data();
	
	s32 FIELD_WIDTH  = game_data->field_dimensions.x;
	s32 FIELD_HEIGHT = game_data->field_dimensions.y;

	// clear field
	bool should_clear_field = input::get_pressed(Keyboard_Keys::Escape);
	if (should_clear_field) {
		field::reset(game_data);
		field::apply_prefab(game_data, figure_prefab_pulsar);
	}

	// toggle cells
	bool should_toggle_cell = input::get_pressed(Pointer_Keys::Key1);
	if (should_toggle_cell) {
		Vector2i pointer = input::get_pointer();
		Vector2i cell = {
			(s32)(pointer.x / CELL_SIZE_POSITION.x),
			(s32)(pointer.y / CELL_SIZE_POSITION.y),
		};
		field::set_cell(game_data, cell, !game_data->field[cell.y * FIELD_WIDTH + cell.x]);
	}

	// toggle active
	bool should_toggle_activity = input::get_pressed(Keyboard_Keys::Space);
	if (should_toggle_activity) {
		game_data->is_active = !game_data->is_active;
		game_data->step_time = 0;
	}

	// update field
	if (game_data->is_active) {
		game_data->step_time += globals::delta_time;
		if (game_data->step_time >= TIME_STEP) {
			game_data->step_time -= TIME_STEP;
			game::update(game_data);
		}
	}

	// request render buffer size
	platform_data->render_settings.target_size = {
		(s32)(FIELD_WIDTH * CELL_SIZE_POSITION.x),
		(s32)(FIELD_HEIGHT * CELL_SIZE_POSITION.y)
	};
	platform_data->render_settings.size_mode = Render_Settings::Size_Mode::Game;
	platform_data->render_settings.stretch_mode = Render_Settings::Stretch_Mode::Fractional;
}

extern "C" CUSTOM_DLL GAME_RENDER(game_render) {
	clear_buffer(globals::render_buffer, {0, 0, 0, 0});

	Vector2 cell_offset_background = (CELL_SIZE_POSITION - CELL_SIZE_BACKGROUND) / 2;
	Vector2 cell_offset_figure = (CELL_SIZE_POSITION - CELL_SIZE_FIGURE) / 2;

	auto game_data = get_game_data();
	
	s32 FIELD_WIDTH  = game_data->field_dimensions.x;
	s32 FIELD_HEIGHT = game_data->field_dimensions.y;

	// draw background
	for (s32 y = 0; y < FIELD_HEIGHT; ++y) {
		for (s32 x = 0; x < FIELD_WIDTH; ++x) {
			bool isOdd = (x + y) % 2;
			Vector4 color = isOdd ? color_background_odd : color_background_even;
			Vector2 xy = {(r32)x, (r32)y};
			Vector2 position = (xy * CELL_SIZE_POSITION) + cell_offset_background;
			draw_rectangle_over(globals::render_buffer, position, CELL_SIZE_BACKGROUND, color);
		}
	}
	
	// draw field
	for (s32 y = 0; y < FIELD_HEIGHT; ++y) {
		for (s32 x = 0; x < FIELD_WIDTH; ++x) {
			bool is_filled = game_data->field[y * FIELD_WIDTH + x];
			if (is_filled) {
				Vector2 xy = {(r32)x, (r32)y};
				Vector2 position = (xy * CELL_SIZE_POSITION) + cell_offset_figure;
				draw_rectangle_over(globals::render_buffer, position, CELL_SIZE_FIGURE, color_field);
			}
		}
	}
	
	// draw pointer
	Vector2i pointer = input::get_pointer();
	Vector2 pointer_position = {
		(r32)pointer.x - CELL_SIZE_FIGURE.x / 2,
		(r32)pointer.y - CELL_SIZE_FIGURE.y / 2
	};
	draw_rectangle(globals::render_buffer, pointer_position, CELL_SIZE_FIGURE, color_pointer);
}

// extern "C" CUSTOM_DLL GAME_OUTPUT_SOUND(game_output_sound) {
// 	auto game_data = get_game_data();
// }
