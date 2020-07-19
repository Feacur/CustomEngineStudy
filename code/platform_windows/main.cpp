#include "main_common.h"

#include <Windows.h>
#include "modules/globals.h"
#include "modules/utilities.h"
#include "modules/render_gdi.h"
// #include "modules/render_opengl.h" // FEATURE_OPENGL
// #include "modules/audio_dsound.h" // FEATURE_DSOUND
#include "modules/game_code.h"
#include "modules/input_raw.h" // FEATURE_RAW_INPUT
// #include "modules/input_xinput.h" // FEATURE_XINPUT
#include "modules/input_keyboard.h"
#include "modules/input_pointer.h"
// #include "modules/input_recording.h" // FEATURE_INPUT_RECORDING
#include "modules/time.h"

#include "modules/api_routines.h"

static Platform_Data platform_data;

extern "C" { // @Note: use discrete GPU by default
	// http://developer.download.nvidia.com/devzone/devcenter/gamegraphics/files/OptimusRenderingPolicies.pdf
	__declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001UL;
	// https://community.amd.com/thread/223376
	// https://gpuopen.com/amdpowerxpressrequesthighperformance/
	__declspec(dllexport) DWORD AmdPowerXpressRequestHighPerformance = 0x00000001UL;
}

//
// Forward declared routines
//

int              platform_windows_main(HINSTANCE hInstance, cstring exe_path);
void             process_messages(HWND window);
LRESULT CALLBACK window_procedure(HWND window, UINT message, WPARAM wParam, LPARAM lParam);
void             toggle_full_screen_mode(HWND window);
void             convert_image();
void             register_raw_input(HWND window);
void             process_raw_input(HWND window, LPARAM lParam);

//
// Entry point
//

// https://docs.microsoft.com/en-us/windows/win32/desktop-programming
// https://docs.microsoft.com/en-us/windows/win32/dlls/dllmain
// https://docs.microsoft.com/en-us/windows/win32/learnwin32/winmain--the-application-entry-point

int WINAPI WinMain(
	HINSTANCE hInstance,     // is something called a "handle to an instance" or "handle to a module." The operating system uses this value to identify the executable (EXE) when it is loaded in memory. The instance handle is needed for certain Windows functions—for example, to load icons or bitmaps.
	HINSTANCE hPrevInstance, // has no meaning. It was used in 16-bit Windows, but is now always zero.
	PSTR      pCmdLine,      // contains the command-line arguments as an ANSI string.
	int       nCmdShow       // is a flag that says whether the main application window will be minimized, maximized, or shown normally.
) {
	win_main_show_console();
	
	char exe_path[MAX_PATH];
	GetModuleFileName(NULL, exe_path, MAX_PATH);

	return platform_windows_main(hInstance, exe_path);
}

// int WINAPI wWinMain(
// 	HINSTANCE hInstance,     // is something called a "handle to an instance" or "handle to a module." The operating system uses this value to identify the executable (EXE) when it is loaded in memory. The instance handle is needed for certain Windows functions—for example, to load icons or bitmaps.
// 	HINSTANCE hPrevInstance, // has no meaning. It was used in 16-bit Windows, but is now always zero.
// 	PWSTR     pCmdLine,      // contains the command-line arguments as a Unicode string.
// 	int       nCmdShow       // is a flag that says whether the main application window will be minimized, maximized, or shown normally.
// ) {
// 	win_main_show_console();
// 	
// 	char exe_path[MAX_PATH];
// 	GetModuleFileName(NULL, exe_path, MAX_PATH);
// 
// 	return platform_windows_main(hInstance, exe_path);
// }

int main(int argc, char * argv[]) {
	HINSTANCE hInstance = GetModuleHandle(NULL);

	// ShowWindow(GetConsoleWindow(), SW_HIDE);
	// ShowWindow(GetConsoleWindow(), SW_SHOW);
	
	return platform_windows_main(hInstance, argv[0]);
}

// int wmain(int argc, wchar_t * argv[]) {
// 	HINSTANCE hInstance = GetModuleHandle(NULL);
// 
// 	// ShowWindow(GetConsoleWindow(), SW_HIDE);
// 	// ShowWindow(GetConsoleWindow(), SW_SHOW);
// 	
// 	return platform_windows_main(hInstance, argv[0]);
// }

// BOOL WINAPI DllMain(
// 	_In_ HINSTANCE hinstDLL, // A handle to the DLL module. The value is the base address of the DLL. The HINSTANCE of a DLL is the same as the HMODULE of the DLL, so hinstDLL can be used in calls to functions that require a module handle.
// 	_In_ DWORD     fdwReason, // The reason code that indicates why the DLL entry-point function is being called. This parameter can be one of the following values.
// 	_In_ LPVOID    lpvReserved
// );

//
// Routines
//

void display_blackness_any(HDC device_context) {
	#if !defined(FEATURE_OPENGL)
	display_blackness(device_context);
	#endif
}

void display_render_buffer_any(HDC device_context) {
	#if defined(FEATURE_OPENGL)
	display_render_buffer_opengl(device_context);
	#else
	display_render_buffer(device_context);
	#endif
}

int platform_windows_main(HINSTANCE hInstance, cstring exe_path) {
	// normalize_directory(exe_path);

	SetConsoleOutputCP(1252);
	// code page 1250: Windows Central Europe
	// code page 1251: Windows Cyrillic
	// code page 1252: Windows Western

	// OutputDebugStringA("Debug message.");

	//
	// Register the window class
	//
	
	WNDCLASS window_class = {};
	window_class.style         = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	window_class.lpfnWndProc   = window_procedure;
	window_class.hInstance     = hInstance;
	window_class.hIcon         = 0;
	window_class.hCursor       = LoadCursor(0, IDC_ARROW);
	window_class.lpszClassName = TEXT("Platform window class");

	ATOM window_class_atom = RegisterClass(&window_class);
	if (!window_class_atom) {
		log_last_error();
		CUSTOM_ASSERT(false, "Can't register the window class");
		return 0;
	}
	
	//
	// Create a window
	//
	
	HWND window = CreateWindowEx(
		0,
		window_class.lpszClassName, TEXT("Platform"),
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		0, 0,
		hInstance,
		0
	);
	if (!window) {
		log_last_error();
		CUSTOM_ASSERT(window, "Can't create a window");
		return 0;
	}
	
	HDC device_context = GetDC(window);

	window_size = get_window_size(window);

	render_settings.size_mode = Render_Settings::Size_Mode::Window;
	render_settings.stretch_mode = Render_Settings::Stretch_Mode::None;
	render_settings.target_size = window_size;

	//
	// Allocate memory
	//
	
	platform_data.permanent_memory.capacity = 100 * 1024 * 1024;
	platform_data.permanent_memory.data = (u8 *)allocate_memory(platform_data.permanent_memory.capacity);
	CUSTOM_ASSERT(platform_data.permanent_memory.data, "Can't allocate permanent memory");
	
	platform_data.transient_memory.capacity = 100 * 1024 * 1024;
	platform_data.transient_memory.data = (u8 *)allocate_memory(platform_data.transient_memory.capacity);
	CUSTOM_ASSERT(platform_data.transient_memory.data, "Can't allocate transient memory");
	
	CUSTOM_MESSAGE("Allocated general memory");
	
	//
	// Initialize performance counters
	//
	
	initialize_time();

	s64 monitor_hz = (s64)GetDeviceCaps(device_context, VREFRESH);
	if (monitor_hz <= 0) { monitor_hz = 60; }

	platform_data.time.precision = 1000000000;
	platform_data.time.target_frame_duration = platform_data.time.precision / monitor_hz;
	
	// GetSystemMetrics with SM_CXSCREEN and SM_CYSCREEN
	monitor_size = {
		(s32)GetDeviceCaps(device_context, HORZRES),
		(s32)GetDeviceCaps(device_context, VERTRES)
	};
	
	CUSTOM_MESSAGE("Initialized performance counters");

	//
	// Initialize OpenGL
	//
	
	#if defined(FEATURE_OPENGL)
	init_opengl(device_context);
	HGLRC rendering_context = wglGetCurrentContext();
	glGenTextures(1, &texture_handle);
	#endif
	
	//
	// Process input
	//
	
	GetCursorPos(&os_input_pointer_screen);
	os_input_pointer_client = os_input_pointer_screen;
	ScreenToClient(window, &os_input_pointer_client);
	
	register_raw_input(window);
	
	#if defined(FEATURE_XINPUT)
	load_xinput();
	#endif

	#if !defined(FEATURE_RAW_INPUT)
	keyboard_mode         = Keyboard_Mode::Message;
	pointer_position_mode = Pointer_Mode::Direct;
	pointer_keys_mode     = Pointer_Mode::Direct;
	#endif
	
	CUSTOM_MESSAGE("Started main cycle");

	game_code = {};
	platform_data.keep_alive = true;
	platform_data.render_settings.size_mode = render_settings.size_mode;
	platform_data.render_settings.stretch_mode = render_settings.stretch_mode;

	static char header_text[128];
	while(platform_data.keep_alive) {
		// idle
		platform_data.time.last_frame_duration = wait_for_next_frame(platform_data.time.target_frame_duration, platform_data.time.precision);
		platform_data.time.since_start = get_clock_span_get_clock_spanseconds(clock_game_start, clock_current, platform_data.time.precision);
		platform_data.time.frame_timestamp = get_timestamp();

		#if !defined(CUSTOM_SHIPPING)
		float debug_ms = platform_data.time.last_frame_duration * 1000LL / (float)platform_data.time.precision;
		float debug_fps = platform_data.time.precision / (float)platform_data.time.last_frame_duration;
		sprintf(header_text, "Platform - %.1f ms (%.1f FPS)", debug_ms, debug_fps);
		SetWindowText(window, header_text);
		#endif
		
		// init
		if (render_settings.fullscreen != platform_data.render_settings.fullscreen) {
			render_settings.fullscreen = platform_data.render_settings.fullscreen;
			toggle_full_screen_mode(window);
		}

		if (render_settings.size_mode != platform_data.render_settings.size_mode) {
			render_settings.size_mode = platform_data.render_settings.size_mode;
			if (render_settings.size_mode == Render_Settings::Size_Mode::Window) {
				render_settings.target_size = window_size;
			}
		}

		if (render_settings.stretch_mode != platform_data.render_settings.stretch_mode) {
			render_settings.stretch_mode = platform_data.render_settings.stretch_mode;
			make_render_buffer_fit(window);
			display_blackness_any(device_context);
		}
		
		if (render_settings.size_mode == Render_Settings::Size_Mode::Game) {
			render_settings.target_size = platform_data.render_settings.target_size;
		}

		reinit_render_buffer(window);
		platform_data.render_buffer_image = render_buffer.image;
		platform_data.render_buffer_image_f = render_buffer.image_f;
		
		reinit_game_code(&game_code);
		platform_data.read_file       = &platform_read_file;
		platform_data.allocate_memory = &platform_allocate_memory;
		platform_data.free_memory     = &platform_free_memory;
		
		#if defined(FEATURE_DSOUND)
		reinit_sound_buffer(window);
		play_sound_buffer();
		platform_data.sound_buffer_sound = sound_buffer.sound;
		#endif
		
		// input
		keyboard_update_previous_state();
		pointer_update_previous_state();

		process_direct_pointer(window);
		process_messages(window);

		#if defined(FEATURE_XINPUT)
		process_xinput_example();
		#endif
		
		platform_data.input_keyboard = input_keyboard;
		platform_data.input_pointer = input_pointer;

		// logic
		#if defined(FEATURE_INPUT_RECORDING)
		process_input_recording(&platform_data.permanent_memory, 0);
		#endif
		(*game_code.game_update)(&platform_data);
		
		// rendering
		(*game_code.game_render)(&platform_data);
		convert_image();

		display_render_buffer_any(device_context);
		
		// sound
		#if defined(FEATURE_DSOUND)
		DWORD play_cursor;
		DWORD write_cursor;
		HRESULT result_GetCurrentPosition = sound_buffer.secondary_buffer->GetCurrentPosition(&play_cursor, &write_cursor);
		if (result_GetCurrentPosition == DS_OK) {
			s32 bytes_for_samples = sound_buffer.sound.channels * sound_buffer.bytes_per_sample;
			s32 bytes_for_second  = bytes_for_samples * sound_buffer.sound.samples_per_second;
			s32 bytes_for_buffer  = (s32)(bytes_for_second * sound_buffer.buffer_length_in_seconds);
			
			s64 time_ahead  = platform_data.time.target_frame_duration * 4;
			s32 bytes_ahead = (s32)mul_div((s64)bytes_for_second, time_ahead, platform_data.time.precision);
			
			DWORD end_at_byte    = (write_cursor + bytes_ahead) % bytes_for_buffer;
			DWORD bytes_to_write = (
				bytes_for_buffer - sound_buffer.running_sample_byte + end_at_byte
			) % bytes_for_buffer;
		
			(*game_code.game_output_sound)(&platform_data, bytes_to_write / bytes_for_samples);
			fill_sound_buffer(bytes_to_write);
		}
		#endif
	}
	
	remove_runtime_game_code(&game_code);
	ReleaseDC(window, device_context);
	
	#if defined(FEATURE_OPENGL)
	wglDeleteContext(rendering_context);
	#endif

	DestroyWindow(window);
	
	CUSTOM_MESSAGE("Finished running");
	return 0;
}

void process_messages(HWND window) {
	MSG message;
	while(PeekMessage(&message, 0, 0, 0, PM_REMOVE)) {
		if (message.message == WM_QUIT) {
			// Indicates a request to terminate an application.
			platform_data.shutdown_request = true;
			break;
		}
		
		TranslateMessage(&message);
		DispatchMessage(&message);
	}
}

#if defined(FEATURE_RAW_INPUT)
void process_raw_input_callback(HWND window, RAWHID const & data) { }
#else
void register_raw_input(HWND window) { }
void process_raw_input(HWND window, LPARAM lParam) { }
#endif

LRESULT CALLBACK window_procedure(
	HWND   window,
	UINT   message,
	WPARAM wParam,
	LPARAM lParam
) {
	static bool window_is_minimized = false;
	static bool window_is_maximized = false;
	static bool window_has_focus    = false;
	static bool inner_area_focus    = false;

	switch (message) {
		// case WM_CHAR: {
		// 	// requires WM_KEYDOWN translated and dispatched
		// 	printf("%C\n", (wchar_t)wParam);
		// } break;
		
		case WM_INPUT: {
			process_raw_input(window, lParam);
			// If an application processes this message, it should return zero.
			return 0;
		} break;
		
		case WM_SYSKEYUP:
		case WM_SYSKEYDOWN:
		case WM_KEYUP:
		case WM_KEYDOWN: {
			process_message_keyboard(window, wParam, lParam);
			// An application should return zero if it processes this message.
			return 0;
		} break;

		case WM_MOUSEMOVE:
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_MBUTTONDOWN:
		case WM_MBUTTONUP:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP: {
			process_message_pointer(window, wParam, lParam);
			// If an application processes this message, it should return zero.
			return 0;
		} break;
		
		case WM_MOUSEWHEEL: {
			auto delta = GET_WHEEL_DELTA_WPARAM(wParam);
			input_pointer.wheel.y = (r32)delta / WHEEL_DELTA;
			// If an application processes this message, it should return zero.
			return 0;
		} break;
		
		case WM_MOUSEHWHEEL: {
			auto delta = GET_WHEEL_DELTA_WPARAM(wParam);
			input_pointer.wheel.x = (r32)delta / WHEEL_DELTA;
			// If an application processes this message, it should return zero.
			return 0;
		} break;

		case WM_SETFOCUS: {
			// Sent to a window after it has gained the keyboard focus.
			window_has_focus      = true;
			input_keyboard_enable = true;
			input_pointer_enable  = true;
			// An application should return zero if it processes this message.
			return 0;
		} break;
		
		case WM_KILLFOCUS: {
			// Sent to a window immediately before it loses the keyboard focus.
			window_has_focus      = false;
			input_keyboard_enable = false;
			input_pointer_enable  = false;
			keyboard_reset_state();
			pointer_reset_state();
			// An application should return zero if it processes this message.
			return 0;
		} break;
		
		case WM_SIZE: {
			// Sent to a window after its size has changed.
			if (wParam == SIZE_MINIMIZED) {
				window_is_minimized = true;
				window_is_maximized = false;
			}
			else if (wParam == SIZE_MAXIMIZED) {
				window_is_minimized = false;
				window_is_maximized = true;
				window_size = get_window_size(window);
				make_render_buffer_fit(window);
			}
			else if (wParam == SIZE_RESTORED) {
				if (window_is_minimized) {
					window_is_minimized = false;
				}
				else if (window_is_maximized) {
					window_is_maximized = false;
					window_size = get_window_size(window);
					make_render_buffer_fit(window);
				}
			}
			// If an application processes this message, it should return zero.
			return 0;
		} break;
		
		case WM_SIZING: {
			// Sent to a window that the user is resizing.
			window_size = get_window_size(window);
			make_render_buffer_fit(window);
			RedrawWindow(window, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
			// An application should return TRUE if it processes this message.
			return TRUE;
		}
		
		case WM_EXITSIZEMOVE: {
			// Sent one time to a window, after it has exited the moving or sizing modal loop.
			window_size = get_window_size(window);
			make_render_buffer_fit(window);
			if (render_settings.size_mode == Render_Settings::Size_Mode::Window) {
				render_settings.target_size = get_window_size(window);
			}
			// An application should return zero if it processes this message.
			return 0;
		} break;
		
		case WM_SETCURSOR: {
			// Sent to a window if the mouse causes the cursor to move within a window and mouse input is not capture
			bool in_the_client_area = (LOWORD(lParam) == HTCLIENT);
			if (window_has_focus && in_the_client_area) {
				// If an application processes this message, it should return TRUE to halt further processing or FALSE to continue.
				// return TRUE;
			}
			// return FALSE;
		} break;
		
		case WM_PAINT: {
			// Is sent when the system or another application makes a request to paint a portion of an application's window.
			bool has_update_region = GetUpdateRect(window, NULL, false);
			
			if (has_update_region) {
				PAINTSTRUCT paint;
				HDC device_context = BeginPaint(window, &paint);
			
				display_blackness_any(device_context);
				display_render_buffer_any(device_context);

				EndPaint(window, &paint);
			}
			
			// An application returns zero if it processes this message.
			return 0;
		} break;
		
		case WM_CLOSE: {
			// Sent as a signal that a window or an application should terminate.
			// Go to WM_DESTROY
			DestroyWindow(window);
			// If an application processes this message, it should return zero.
			return 0;
		} break;
		
		case WM_DESTROY: {
			// Sent when a window is being destroyed. It is sent to the window procedure of the window being destroyed after the window is removed from the screen.
			// Go to WM_QUIT
			PostQuitMessage(0);
			// If an application processes this message, it should return zero.
			return 0;
		} break;
	}

	return DefWindowProc(window, message, wParam, lParam);
}

void toggle_full_screen_mode(HWND window) {
	// INITIALLY BY Raymond Chen
	// FROM http://blogs.msdn.com/b/oldnewthing/archive/2010/04/12/9994016.aspx
	static WINDOWPLACEMENT normal_window_position;
	
	DWORD window_style = GetWindowLong(window, GWL_STYLE);
	
	if(BITS_ARE_SET(window_style, WS_OVERLAPPEDWINDOW))
	{
		// Go to windowed fullscreen mode if can remember window placement and get monitor info.
		MONITORINFO monitor_info = {};
		monitor_info.cbSize = sizeof(MONITORINFO);
		if(GetWindowPlacement(window, &normal_window_position) && GetMonitorInfo(MonitorFromWindow(window, MONITOR_DEFAULTTOPRIMARY), &monitor_info))
		{
			SetWindowLong(window, GWL_STYLE, window_style & ~WS_OVERLAPPEDWINDOW);
			SetWindowPos(
				window, HWND_TOP,
				monitor_info.rcMonitor.left, monitor_info.rcMonitor.top,
				monitor_info.rcMonitor.right - monitor_info.rcMonitor.left,
				monitor_info.rcMonitor.bottom - monitor_info.rcMonitor.top,
				SWP_NOOWNERZORDER | SWP_FRAMECHANGED
			);
		}
	}
	else
	{
		// Restore windowed mode.
		SetWindowLong(window, GWL_STYLE, window_style | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(window, &normal_window_position);
		SetWindowPos(
			window, 0,
			0, 0, 0, 0,
			SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_FRAMECHANGED
		);
	}
	
	window_size = get_window_size(window);
	make_render_buffer_fit(window);
}

void convert_image() {
	auto exposure = platform_data.render_buffer_image_f.exposure;
	render_buffer.image_f.exposure = exposure;
	if (exposure <= 0) { return; }
	s32 pixels_count = render_buffer.image.size.x * render_buffer.image.size.y;
	for (s32 i = 0; i < pixels_count; ++i) {
		auto color = render_buffer.image_f.data[i] / exposure;
		color = clamp(color, {0, 0, 0}, {1, 1, 1});
		*(render_buffer.image.data + i) = vector4_to_color32(color, render_buffer.image.offsets);
	}
}
