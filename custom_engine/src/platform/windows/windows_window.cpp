#include "custom_pch.h"

#include "engine/core/code.h"
#include "engine/debug/log.h"
#include "engine/api/platform/window.h"
#include "engine/api/platform/graphics_vm.h"
#include "engine/impl/math_bitwise.h"
#include "engine/impl/math_linear.h"

#include "platform/graphics_context.h"

#if !defined(CUSTOM_PRECOMPILED_HEADER)
	#include <Windows.h>
	// #include <windowsx.h>
#endif

#define CUSTOM_WINDOW_PTR "cwptr"

// https://docs.microsoft.com/en-us/windows/win32/api/wingdi/nf-wingdi-getdevicecaps

//
// API implementation
//

static void platform_raw_input_init(HWND hwnd);
static void platform_raw_input_shutdown(HWND hwnd);
static ivec2 platform_get_window_size(HWND hwnd);
static ATOM platform_register_window_class(void);
static HWND platform_create_window(void);
static void platform_toggle_borderless_fullscreen(HWND hwnd);

static inline void keyboard_update(custom::window::Internal_Data * data);
static inline void mouse_update(custom::window::Internal_Data * data);

namespace custom {
namespace window {

struct Internal_Data
{
	HWND hwnd;
	bool is_externally_destroyed;
	context::Internal_Data * graphics_context;
	ivec2 size;

	struct {
		bool keys[(u8)custom::Key_Code::Last];
		bool prev[(u8)custom::Key_Code::Last];
	} keyboard;

	struct {
		bool keys[(u8)custom::Mouse_Code::Last];
		bool prev[(u8)custom::Mouse_Code::Last];
		ivec2 position;
		ivec2 delta;
		vec2 wheel;
		POINT screen;
		POINT client;
	} mouse;

	struct {
		viewport_func * viewport;
		close_func    * close;
	} callbacks;
};

Internal_Data * create(void) {
	// @Bug: is this error prone to register a window class like that?
	platform_register_window_class();

	Internal_Data * data = (Internal_Data *)calloc(1, sizeof(Internal_Data));
	CUSTOM_ASSERT(data, "failed to allocate window");

	data->hwnd = platform_create_window();
	data->size = platform_get_window_size(data->hwnd);
	SetProp(data->hwnd, TEXT(CUSTOM_WINDOW_PTR), data);

	platform_raw_input_init(data->hwnd);

	return data;
}

static void clean_internal(Internal_Data * data) {
	if (data->graphics_context) {
		context::destroy(data->graphics_context);
		data->graphics_context = NULL;
	}
	if (data->hwnd) {
		platform_raw_input_shutdown(data->hwnd);
		RemoveProp(data->hwnd, TEXT(CUSTOM_WINDOW_PTR));
		data->hwnd = NULL;
	}
}

void destroy(Internal_Data * data) {
	if (data->hwnd) { DestroyWindow(data->hwnd); return; }
	free(data);
}

void init_context(Internal_Data * data) {
	CUSTOM_ASSERT(data->hwnd, "window doesn't exist");
	CUSTOM_ASSERT(!data->graphics_context, "trying to create a second rendering context");
	data->graphics_context = context::create(data);
	graphics::init();
}

void update(Internal_Data * data) {
	CUSTOM_ASSERT(data->hwnd, "window doesn't exist");
	// @Todo: split into input consumption and buffer output?
	keyboard_update(data);
	mouse_update(data);
	context::swap_buffers(data->graphics_context);
}

void set_vsync(Internal_Data * data, u8 value) {
	CUSTOM_ASSERT(data->hwnd, "window doesn't exist");
	context::set_vsync(data->graphics_context, value);
}

bool check_vsync(Internal_Data * data) {
	CUSTOM_ASSERT(data->hwnd, "window doesn't exist");
	return context::check_vsync(data->graphics_context);
}

u16 get_refresh_rate(Internal_Data * data, u16 default_value) {
	CUSTOM_ASSERT(data->hwnd, "window doesn't exist");
	s32 monitor_hz = GetDeviceCaps(GetDC(data->hwnd), VREFRESH);
	return (monitor_hz > 0) ? (u16)monitor_hz : default_value;
}

void toggle_borderless_fullscreen(Internal_Data * data) {
	CUSTOM_ASSERT(data->hwnd, "window doesn't exist");
	platform_toggle_borderless_fullscreen(data->hwnd);
}

void set_header(Internal_Data * data, cstring value) {
	CUSTOM_ASSERT(data->hwnd, "window doesn't exist");
	SetWindowText(data->hwnd, value);
}

ivec2 const & get_size(Internal_Data * data) {
	CUSTOM_ASSERT(data->hwnd, "window doesn't exist");
	return data->size;
}

bool get_is_active(Internal_Data * data) {
	return data->hwnd;
}

// input
bool get_key(Internal_Data * data, Key_Code key) {
	CUSTOM_ASSERT(data->hwnd, "window doesn't exist");
	return data->keyboard.keys[(u8)key];
}

bool get_mouse_key(Internal_Data * data, Mouse_Code key) {
	CUSTOM_ASSERT(data->hwnd, "window doesn't exist");
	return data->mouse.keys[(u8)key];
}

bool get_key_transition(Internal_Data * data, Key_Code key, bool to_state) {
	CUSTOM_ASSERT(data->hwnd, "window doesn't exist");
	bool from = data->keyboard.prev[(u8)key];
	bool to = data->keyboard.keys[(u8)key];
	return (from != to) && (to == to_state);
}

bool get_mouse_key_transition(Internal_Data * data, Mouse_Code key, bool to_state) {
	CUSTOM_ASSERT(data->hwnd, "window doesn't exist");
	bool from = data->mouse.prev[(u8)key];
	bool to = data->mouse.keys[(u8)key];
	return (from != to) && (to == to_state);
}

ivec2 const & get_mouse_pos(Internal_Data * data) {
	CUSTOM_ASSERT(data->hwnd, "window doesn't exist");
	return data->mouse.position;
}

ivec2 const & get_mouse_delta(Internal_Data * data) {
	CUSTOM_ASSERT(data->hwnd, "window doesn't exist");
	return data->mouse.delta;
}

vec2 const & get_mouse_wheel(Internal_Data * data) {
	CUSTOM_ASSERT(data->hwnd, "window doesn't exist");
	return data->mouse.wheel;
}

// callbacks
void set_viewport_callback(Internal_Data * data, viewport_func * callback) {
	CUSTOM_ASSERT(data->hwnd, "window doesn't exist");
	data->callbacks.viewport = callback;
}

void set_close_callback(Internal_Data * data, close_func * callback) {
	CUSTOM_ASSERT(data->hwnd, "window doesn't exist");
	data->callbacks.close = callback;
}

// platform internal access
HDC get_hdc(Internal_Data * data) {
	CUSTOM_ASSERT(data->hwnd, "window doesn't exist");
	return GetDC(data->hwnd);
}

}}

//
// platform implementation
//

static ivec2 platform_get_window_size(HWND hwnd) {
	RECT client_rect;
	GetClientRect(hwnd, &client_rect);
	return {client_rect.right, client_rect.bottom};
}

static LRESULT CALLBACK window_procedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
static ATOM platform_register_window_class(void) {
	static ATOM window_class_atom = NULL;
	if (window_class_atom) { return window_class_atom; }

	// https://docs.microsoft.com/en-us/windows/win32/api/winuser/ns-winuser-wndclassexa
	WNDCLASSEX window_class; // = {};
	ZeroMemory(&window_class, sizeof(window_class));
	window_class.cbSize        = sizeof(window_class);
	window_class.style         = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	window_class.lpfnWndProc   = window_procedure;
	window_class.hCursor       = LoadCursor(0, IDC_ARROW);
	window_class.lpszClassName = TEXT(CUSTOM_WINDOW_CLASS_NAME);

	// https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-registerclassexa
	window_class_atom = RegisterClassEx(&window_class);
	CUSTOM_ASSERT(window_class_atom, "failed to register window class");
	return window_class_atom;
}

static HWND platform_create_window(void) {
	DWORD     dwExStyle  = WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR;
	DWORD     dwStyle    = WS_OVERLAPPEDWINDOW | WS_VISIBLE;
	HWND      hWndParent = HWND_DESKTOP;
	HMENU     hMenu      = NULL;
	HINSTANCE hInstance  = GetModuleHandle(NULL);
	LPVOID    lpParam    = NULL;

	// https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-createwindowexa
	HWND hwnd = CreateWindowEx(
		dwExStyle,
		TEXT(CUSTOM_WINDOW_CLASS_NAME), TEXT(""),
		dwStyle,
		// int X, Y, nWidth, nHeight
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		hWndParent, hMenu, hInstance, lpParam
	);
	CUSTOM_ASSERT(hwnd, "failed to create window");
	return hwnd;
}

static void platform_toggle_borderless_fullscreen(HWND hwnd) {
	static WINDOWPLACEMENT normal_window_position;
	
	DWORD window_style = GetWindowLong(hwnd, GWL_STYLE);
	if (BITS_ARE_SET(window_style, WS_OVERLAPPEDWINDOW)) {
		if (!GetWindowPlacement(hwnd, &normal_window_position)) { return; }

		MONITORINFO monitor_info = {};
		monitor_info.cbSize = sizeof(MONITORINFO);
		if (!GetMonitorInfo(MonitorFromWindow(hwnd, MONITOR_DEFAULTTOPRIMARY), &monitor_info)) { return; }

		SetWindowLong(hwnd, GWL_STYLE, window_style & ~WS_OVERLAPPEDWINDOW);
		SetWindowPos(
			hwnd, HWND_TOP,
			monitor_info.rcMonitor.left, monitor_info.rcMonitor.top,
			monitor_info.rcMonitor.right - monitor_info.rcMonitor.left,
			monitor_info.rcMonitor.bottom - monitor_info.rcMonitor.top,
			SWP_NOOWNERZORDER | SWP_FRAMECHANGED
		);
		return;
	}

	// Restore windowed mode
	SetWindowLong(hwnd, GWL_STYLE, window_style | WS_OVERLAPPEDWINDOW);
	SetWindowPlacement(hwnd, &normal_window_position);
	SetWindowPos(
		hwnd, 0,
		0, 0, 0, 0,
		SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_FRAMECHANGED
	);
}

//
// input
//

typedef custom::window::Internal_Data Window;

enum struct Input_Mode {
	Raw,
	Message,
};

static Input_Mode keyboard_mode = Input_Mode::Message;
static Input_Mode mouse_mode    = Input_Mode::Message;

#include "windows_input_raw.h"
#include "windows_input_keyboard.h"
#include "windows_input_mouse.h"

#if !defined(CUSTOM_FEATURE_RAW_INPUT)
	static void platform_raw_input_init(HWND hwnd) { /**/ }
	static void platform_raw_input_shutdown(HWND hwnd) { /**/ }
	static void process_message_raw(Window * window, WPARAM wParam, LPARAM lParam) { /**/ }
#else
	static void raw_input_callback(Window * window, RAWHID const & data) { /*not implemented*/ }
#endif

//
// window procedure callback
// https://docs.microsoft.com/en-us/previous-versions/windows/desktop/legacy/ms633573(v=vs.85)?redirectedfrom=MSDN
//

static LRESULT CALLBACK window_procedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	Window * window = (Window *)GetProp(hwnd, TEXT(CUSTOM_WINDOW_PTR));
	if (!window) {
		return DefWindowProc(hwnd, message, wParam, lParam);
	}
	CUSTOM_ASSERT(window->hwnd, "window does't exist");

	switch (message) {
		// https://docs.microsoft.com/en-us/windows/win32/inputdev/raw-input
		case WM_INPUT: {
			process_message_raw(window, wParam, lParam);
			return 0; // If an application processes this message, it should return zero.
		} break;

		// https://docs.microsoft.com/en-us/windows/win32/inputdev/keyboard-input
		case WM_SYSKEYUP:
		case WM_SYSKEYDOWN:
		case WM_KEYUP:
		case WM_KEYDOWN: {
			process_message_keyboard(window, wParam, lParam);
			// @Note: [alt]+[f4] won't result with a WM_CLOSE if messages are not passed through
			return 0; // An application should return zero if it processes this message.
		} break;
		
		// case WM_CHAR: {
		// 	return 0; // An application should return zero if it processes this message.
		// } break;

		// case WM_SETFOCUS: {
		// 	// Sent to a window after it has gained the keyboard focus.
		// 	return 0; // An application should return zero if it processes this message.
		// } break;
		
		case WM_KILLFOCUS: {
			// Sent to a window immediately before it loses the keyboard focus.
			keyboard_reset(window);
			mouse_reset(window);
			return 0; // An application should return zero if it processes this message.
		} break;

		// https://docs.microsoft.com/en-us/windows/win32/inputdev/mouse-input
		case WM_MOUSEMOVE:
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_MBUTTONDOWN:
		case WM_MBUTTONUP:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
		case WM_XBUTTONDOWN:
		case WM_XBUTTONUP: {
			process_message_mouse(window, wParam, lParam, true, {0, 0}); // @Note: client-space, NO wheel
			return 0; // If an application processes this message, it should return zero.
		} break;

		case WM_MOUSEWHEEL: {
			process_message_mouse(window, wParam, lParam, false, {0, 1}); // @Note: screen-space, Y wheel
			return 0; // If an application processes this message, it should return zero.
		} break;
		
		case WM_MOUSEHWHEEL: {
			process_message_mouse(window, wParam, lParam, false, {1, 0}); // @Note: screen-space, X wheel
			return 0; // If an application processes this message, it should return zero.
		} break;

		// https://docs.microsoft.com/en-us/windows/win32/menurc/cursor-notifications
		// case WM_SETCURSOR: {
		// 	// Sent to a window if the mouse causes the cursor to move within a window and mouse input is not capture
		// 	WORD hit_test = LOWORD(lParam);
		// 	WORD source_message = HIWORD(lParam);
		// 	return TRUE; // If an application processes this message, it should return TRUE to halt further processing or FALSE to continue.
		// } break;

		// https://docs.microsoft.com/en-us/windows/win32/gdi/painting-and-drawing-messages
		// case WM_PAINT: {
		// 	// Is sent when the system or another application makes a request to paint a portion of an application's window.
		// 	bool has_update_region = GetUpdateRect(hwnd, NULL, false);
		// 	if (has_update_region) {
		// 		PAINTSTRUCT paint;
		// 		HDC hdc = BeginPaint(hwnd, &paint);
		// 		EndPaint(hwnd, &paint);
		// 	}
		// 	return 0; // An application returns zero if it processes this message.
		// } break;

		// https://docs.microsoft.com/en-us/windows/win32/winmsg/window-notifications
		// case WM_CREATE: {
		// 	CREATESTRUCT * data = (CREATESTRUCTA *)lParam;
		// 	return 0; // If an application processes this message, it should return zero to continue creation of the window.
		// } break;

		case WM_SIZE: {
			// Sent to a window after its size has changed.
			window->size = {LOWORD(lParam), HIWORD(lParam)};
			CALL_SAFELY(window->callbacks.viewport, window, window->size);
			if (wParam == SIZE_MINIMIZED) {
			}
			else if (wParam == SIZE_MAXIMIZED) {
			}
			else if (wParam == SIZE_RESTORED) {
			}
			return 0; // If an application processes this message, it should return zero.
		} break;

		// case WM_SIZING: {
		// 	// Sent to a window that the user is resizing.
		// 	// wParam
		// 	// WMSZ_BOTTOM
		// 	// WMSZ_BOTTOMLEFT
		// 	// WMSZ_BOTTOMRIGHT
		// 	// WMSZ_LEFT
		// 	// WMSZ_RIGHT
		// 	// WMSZ_TOP
		// 	// WMSZ_TOPLEFT
		// 	// WMSZ_TOPRIGHT
		// 	RECT * rect = (RECT *)lParam;
		// 	return TRUE; // An application should return TRUE if it processes this message.
		// }

		// case WM_ENTERSIZEMOVE: {
		// 	// Sent one time to a window after it enters the moving or sizing modal loop.
		// 	return 0; // An application should return zero if it processes this message.
		// } break;

		// case WM_EXITSIZEMOVE: {
		// 	// Sent one time to a window, after it has exited the moving or sizing modal loop.
		// 	return 0; // An application should return zero if it processes this message.
		// } break;

		// @Note: forgo the normal pipeline and just prompt the window to close
		case WM_CLOSE: {
			// Sent as a signal that a window or an application should terminate.
			if (window->callbacks.close) {
				(*window->callbacks.close)(window);
			}
			else {
				window->is_externally_destroyed = true;
				DestroyWindow(hwnd); // Go to WM_DESTROY
			}
			return 0; // If an application processes this message, it should return zero.
		} break;

		// @Note: forgo the normal pipeline and let the application decide
		case WM_DESTROY: {
			// Sent when a window is being destroyed. It is sent to the window procedure of the window being destroyed after the window is removed from the screen.
			if (window->is_externally_destroyed) {
				custom::window::clean_internal(window);
			}
			else {
				free(window);
			}
			// 	PostQuitMessage(0); // Go to WM_QUIT
			return 0; // If an application processes this message, it should return zero.
		} break;
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}
