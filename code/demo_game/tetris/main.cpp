#include "demo_game/main_common.h"
#include "demo_game/platform_globals.h"

#include "shared/software_renderer_rect.h"
#include "shared/software_renderer_simd.h"

#include "data.h"
#include "code.h"

extern "C" CUSTOM_DLL GAME_UPDATE(game_update) {
	globals::cache(platform_data);

	auto game_data = get_game_data();
	
	s32 FIELD_WIDTH  = game_data->field_dimensions.x;
	s32 FIELD_HEIGHT = game_data->field_dimensions.y;

	// creation
	if (game_data->figure.length == 0) {
		game_data->position = {FIELD_WIDTH / 2, FIELD_HEIGHT - 1};

		Array_Dynamic<Vector2i> figure;
		figure.data = (Vector2i *)figure_prefabs[random_in_range(&game_data->random_state, 0, FIGURES_NUMBER)];
		figure.capacity = FIGURE_PREFAB_LENGTH;
		figure.length = FIGURE_PREFAB_LENGTH;
		game::set_figure(game_data, figure);
		
		if (game_data->figure.length == 0) {
			field::reset(game_data);
		}
	}

	// movement
	
	if (game_data->figure.length > 0) {
		game_data->step_time_side += globals::delta_time;
		game_data->step_time_down += globals::delta_time;

		// rotate
		bool should_rotate_figure = input::get_pressed(Keyboard_Keys::Up);
		if (should_rotate_figure) {
			game::rotate_the_figure(game_data);
		}

		// move sideways
		if (game_data->step_time_side >= TIME_STEP_SIDE) {
			game_data->step_time_side -= TIME_STEP_SIDE;
			
			bool should_move_left = input::get_current(Keyboard_Keys::Left);
			if (should_move_left && game::move_is_valid(game_data, game_data->figure, {-1, 0})) {
				game_data->position.x -= 1;
			}
			
			bool should_move_right = input::get_current(Keyboard_Keys::Right);
			if (should_move_right && game::move_is_valid(game_data, game_data->figure, {1, 0})) {
				game_data->position.x += 1;
			}
		}

		// move down
		bool should_move_fast = input::get_current(Keyboard_Keys::Down);
		s32 time_step_down = should_move_fast ? TIME_STEP_DOWN_FAST : TIME_STEP_DOWN_NORMAL;

		if (game_data->step_time_down >= time_step_down) {
			game_data->step_time_down -= time_step_down;
			
			if (game::move_is_valid(game_data, game_data->figure, {0, -1})) {
				game_data->position.y -= 1;
			}
			else {
				field::imprint_figure(game_data);
				field::collapse(game_data);
			}
		}
	}

	// request render buffer size
	platform_data->render_settings.target_size = game_data->field_dimensions * TILE_SIZE;
	platform_data->render_settings.size_mode = Render_Settings::Size_Mode::Game;
	platform_data->render_settings.stretch_mode = Render_Settings::Stretch_Mode::Fractional;
}

extern "C" CUSTOM_DLL GAME_RENDER(game_render) {
	clear_buffer(globals::render_buffer, {0, 0, 0, 0});

	auto game_data = get_game_data();
	
	s32 FIELD_WIDTH  = game_data->field_dimensions.x;
	s32 FIELD_HEIGHT = game_data->field_dimensions.y;

	s32 scale = max(min(globals::render_buffer.size / (game_data->field_dimensions * TILE_SIZE)), 1);
	Vector2i TILE_SIZE_S = TILE_SIZE * scale;
	Vector2i CELL_SIZE_S = CELL_SIZE * scale;

	// draw background
	Vector2 tile_size = {(r32)TILE_SIZE_S.x, (r32)TILE_SIZE_S.y};
	Vector2 cell_size = {(r32)CELL_SIZE_S.x, (r32)CELL_SIZE_S.y};
	Vector2i cell_offset = (TILE_SIZE_S - CELL_SIZE_S) / 2;
	for (s32 y = 0; y < FIELD_HEIGHT; ++y) {
		for (s32 x = 0; x < FIELD_WIDTH; ++x) {
			Vector2i base_xy = vec2i(x, y) * TILE_SIZE_S;
			
			Vector2i xy = base_xy;
			Vector2 position = {(r32)xy.x, (r32)xy.y};

			Vector4 color = ((x + y) % 2) ? color_background_odd : color_background_even;
			draw_rectangle_over(globals::render_buffer, position, tile_size, color);
		}
	}
	
	// draw field
	for (s32 y = 0; y < FIELD_HEIGHT; ++y) {
		for (s32 x = 0; x < FIELD_WIDTH; ++x) {
			bool is_filled = game_data->field[y * FIELD_WIDTH + x];
			if (is_filled) {
				Vector2i base_xy = vec2i(x, y) * TILE_SIZE_S;
				
				Vector2i xy = base_xy + cell_offset;
				Vector2 position = {(r32)xy.x, (r32)xy.y};

				draw_rectangle_over(globals::render_buffer, position, cell_size, color_field);
			}
		}
	}
	
	// draw figure
	for (s32 i = 0; i < game_data->figure.length; ++i) {
		Vector2i position_i = game_data->figure[i] + game_data->position;
		Vector2i base_xy = position_i * TILE_SIZE_S;
		
		Vector2i xy = base_xy + cell_offset;
		Vector2 position = {(r32)xy.x, (r32)xy.y};

		draw_rectangle_over(globals::render_buffer, position, cell_size, color_figure);
	}
}

// extern "C" CUSTOM_DLL GAME_OUTPUT_SOUND(game_output_sound) {
// 	auto game_data = get_game_data();
// }
