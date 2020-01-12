#include "demo_game/main_common.h"
#include "demo_game/platform_globals.h"

#include "shared/software_renderer_rect.h"
#include "shared/software_renderer_simd.h"

#include "data.h"
#include "data_prefabs.h"
#include "code.h"
#include "code_debug.h"

// https://gameinternals.com/understanding-pac-man-ghost-behavior
// https://www.gamasutra.com/view/feature/3938/the_pacman_dossier.php?print=1
// https://youtu.be/ataGotQ7ir8
// https://youtu.be/G8W7EQKBgcg
// https://youtu.be/9bbhJi0NBkk
// https://youtu.be/w5kFmdkrIuY

extern "C" CUSTOM_DLL GAME_UPDATE(game_update) {
	globals::cache(platform_data);

	auto game_data = get_game_data();
	
	game::update(game_data);
	
	// request render buffer size
	platform_data->render_settings.target_size = game_data->map_dimensions * TILE_SIZE;
	platform_data->render_settings.size_mode = Render_Settings::Size_Mode::Game;
	platform_data->render_settings.stretch_mode = Render_Settings::Stretch_Mode::Fractional;
}

extern "C" CUSTOM_DLL GAME_RENDER(game_render) {
	clear_buffer(globals::render_buffer, {0, 0, 0, 0});

	auto game_data = get_game_data();
	
	s32 MAP_WIDTH  = game_data->map_dimensions.x;
	s32 MAP_HEIGHT = game_data->map_dimensions.y;

	s32 scale = max(min(globals::render_buffer.size / (game_data->map_dimensions * TILE_SIZE)), 1);
	Vector2i TILE_SIZE_S      = TILE_SIZE      * scale;
	Vector2i DOT_SIZE_S       = DOT_SIZE       * scale;
	Vector2i ENERGY_SIZE_S    = ENERGY_SIZE    * scale;
	Vector2i CHARACTER_SIZE_S = CHARACTER_SIZE * scale;

	// draw map
	Vector2 tile_size = {(r32)TILE_SIZE_S.x, (r32)TILE_SIZE_S.y};
	Vector2 dot_size = {(r32)DOT_SIZE_S.x, (r32)DOT_SIZE_S.y};
	Vector2i dot_offset = (TILE_SIZE_S - DOT_SIZE_S) / 2;
	Vector2 energy_size = {(r32)ENERGY_SIZE_S.x, (r32)ENERGY_SIZE_S.y};
	Vector2i energy_offset = (TILE_SIZE_S - ENERGY_SIZE_S) / 2;
	for (s32 y = 0; y < MAP_HEIGHT; ++y) {
		for (s32 x = 0; x < MAP_WIDTH; ++x) {
			Vector2i base_xy = vec2i(x, y) * TILE_SIZE_S;

			Tile tile_type = map::get_tile(game_data, y * MAP_WIDTH + x);
			Vector4 tile_color = TILE_COLORS[tile_type];
			if (tile_color.w != 0.0f) {
				Vector2i xy = base_xy;
				Vector2 position = {(r32)xy.x, (r32)xy.y};
				draw_rectangle_over(globals::render_buffer, position, tile_size, tile_color);
			}

			if (bits_are_set(TILE_FLAGS[tile_type], Tile_Type::Dot)) {
				Vector2i xy = base_xy + dot_offset;
				Vector2 position = {(r32)xy.x, (r32)xy.y};
				draw_rectangle_over(globals::render_buffer, position, dot_size, DOT_COLOR);
			}

			if (bits_are_set(TILE_FLAGS[tile_type], Tile_Type::Energy)) {
				Vector2i xy = base_xy + energy_offset;
				Vector2 position = {(r32)xy.x, (r32)xy.y};
				draw_rectangle_over(globals::render_buffer, position, energy_size, ENERGY_COLOR);
			}
		}
	}
	
	// draw characters
	Vector2 character_size = {(r32)CHARACTER_SIZE_S.x, (r32)CHARACTER_SIZE_S.y};
	Vector2i character_offset = (TILE_SIZE_S - CHARACTER_SIZE_S) / 2;
	for (u8 i = 0; i < game_data->characters.length; ++i) {
		if (!CHARACTER_TYPES[i]) { continue; }

		Character character = game_data->characters[i];
		Timer timer = game_data->characters_timers[i];
		u8 type = CHARACTER_TYPES[i];

		Vector2i xy = character.position * TILE_SIZE_S + character_offset;
		Vector2i next_position = utils::wrap_position(character.position + character.direction, game_data->map_dimensions);
		if (map::is_walkable(game_data, next_position)) {
			Vector2i move_offset = character.direction * TILE_SIZE_S;
			xy += move_offset * timer.elapsed / timer.period;
		}

		Vector2 position = {(r32)xy.x, (r32)xy.y};
		Vector4 character_color = CHARACTER_COLORS[i];
		if ((game_data->mode == Mode::Fright) && (CHARACTER_TEAM[i] == 1)) {
			character_color = FRIGHTENED_COLOR;
		}
		draw_rectangle_over(globals::render_buffer, position, character_size, character_color);

		static Vector4 const head_color = {0.5f, 0.5f, 0.5f, 1.0f};
		Vector2 head_position = position + vec2(
			(1 + character.direction.x) * character_size.x / 4,
			(1 + character.direction.y) * character_size.y / 4
		);
		draw_rectangle_over(globals::render_buffer, head_position, character_size / 2, head_color);
	}
}

// extern "C" CUSTOM_DLL GAME_OUTPUT_SOUND(game_output_sound) {
// 	auto game_data = get_game_data();
// }
