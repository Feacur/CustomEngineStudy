struct Time {
	u64 frame_timestamp;
	s64  precision;
	s64  target_frame_duration;
	s64  last_frame_duration;
	s64  since_start;
};

struct Sound_Data {
	s16  * data;
	u32   size;
	s32    channels;
	s32    samples_per_second;
};

struct Render_Settings {
	enum struct Size_Mode : u8 {
		Game,
		Window,
	};

	enum struct Stretch_Mode : u8 {
		None,
		Fractional,
	};

	Vector2i     target_size;
	Size_Mode    size_mode;
	Stretch_Mode stretch_mode;
	bool         fullscreen;
};
UNDERLYING_TYPE_META(Render_Settings::Size_Mode, u8)
IS_ENUM_META(Render_Settings::Size_Mode)
UNDERLYING_TYPE_META(Render_Settings::Stretch_Mode, u8)
IS_ENUM_META(Render_Settings::Stretch_Mode)

//
//
//

#define PLATFORM_READ_FILE(ROUTINE_NAME) u8 * ROUTINE_NAME(Memory_Chunk * memory, cstring file_name)
typedef PLATFORM_READ_FILE(platform_read_file_func);

#define PLATFORM_ALLOCATE_MEMORY(ROUTINE_NAME) void * ROUTINE_NAME(size_t size_in_bytes)
typedef PLATFORM_ALLOCATE_MEMORY(platform_allocate_memory_func);

#define PLATFORM_REALLOCATE_MEMORY(ROUTINE_NAME) void * ROUTINE_NAME(void * memory, size_t size_in_bytes)
typedef PLATFORM_REALLOCATE_MEMORY(platform_reallocate_memory_func);

#define PLATFORM_FREE_MEMORY(ROUTINE_NAME) void ROUTINE_NAME(void * memory)
typedef PLATFORM_FREE_MEMORY(platform_free_memory_func);

struct Platform_Data {
	// pre-allocated memory chunks
	Memory_Chunk     permanent_memory;
	Memory_Chunk     transient_memory;
	// data from platform to application layer
	bool             shutdown_request;
	Time             time;
	Input_Keyboard   input_keyboard;
	Input_Pointer    input_pointer;
	// data from application to platform layer
	bool             keep_alive;
	Render_Settings  render_settings;
	RGBA_Data        render_buffer_image;
	Image_Data       render_buffer_image_f;
	Sound_Data       sound_buffer_sound;
	// platform-specific routines
	platform_read_file_func         * read_file;
	platform_allocate_memory_func   * allocate_memory;
	platform_reallocate_memory_func * reallocate_memory;
	platform_free_memory_func       * free_memory;
};

#define GAME_UPDATE(ROUTINE_NAME)       void ROUTINE_NAME(Platform_Data * platform_data)
#define GAME_RENDER(ROUTINE_NAME)       void ROUTINE_NAME(Platform_Data * platform_data)
#define GAME_OUTPUT_SOUND(ROUTINE_NAME) void ROUTINE_NAME(Platform_Data * platform_data, s32 samples_count)
