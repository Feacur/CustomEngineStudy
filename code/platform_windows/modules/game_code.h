static cstring const build_library_name   = "demo_game.dll";
static cstring const runtime_library_name = "demo_game_runtime.dll";

typedef GAME_UPDATE(game_update_type);
typedef GAME_RENDER(game_render_type);
typedef GAME_OUTPUT_SOUND(game_output_sound_type);

GAME_UPDATE(game_update_stub) { }
GAME_RENDER(game_render_stub) { }
GAME_OUTPUT_SOUND(game_output_sound_stub) { }

struct Game_Code {
	HMODULE                  library;
	FILETIME                 creation_time;
	game_update_type       * game_update;
	game_render_type       * game_render;
	game_output_sound_type * game_output_sound;
};

static Game_Code game_code;

//
// Routines
//

#define LOAD_PROCEDURE(ROUTINE_NAME)\
if (code->library) {\
	code->ROUTINE_NAME = (ROUTINE_NAME##_type *)GetProcAddress(code->library, #ROUTINE_NAME);\
}\
else {\
	code->ROUTINE_NAME = NULL;\
}\
if (!code->ROUTINE_NAME) {\
	LOG_WARNING("Can't find \"" #ROUTINE_NAME "\" handler in the game code library");\
	code->ROUTINE_NAME = &ROUTINE_NAME##_stub;\
}

void load_game_code(Game_Code * code, cstring file_name) {
	code->library = LoadLibrary(file_name);
	LOAD_PROCEDURE(game_update)
	LOAD_PROCEDURE(game_render)
	LOAD_PROCEDURE(game_output_sound)
}

#undef LOAD_PROCEDURE

void reinit_game_code(Game_Code * code) {
	WIN32_FIND_DATA find_data;
	HANDLE file = FindFirstFile(build_library_name, &find_data);
	ASSERT_TRUE(file != INVALID_HANDLE_VALUE, "Can't find game code library");

	auto creation_time = find_data.ftLastWriteTime;
	FindClose(file);

	if (CompareFileTime(&code->creation_time, &creation_time) != 0) {
		FreeLibrary(code->library);
		while (!CopyFile(build_library_name, runtime_library_name, 0)) {
			log_last_error();
			Sleep(100);
		}

		load_game_code(code, runtime_library_name);
		code->creation_time = creation_time;
		LOG_TRACE("Reloaded game code library");
	}
}

void remove_runtime_game_code(Game_Code * code) {
	FreeLibrary(code->library);
	DeleteFile(runtime_library_name);
}
