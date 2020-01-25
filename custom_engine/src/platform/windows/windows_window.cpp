#include "custom_pch.h"
#include "engine/api/window.h"
#include "platform/opengl_context.h"
#include "engine/core/code.h"
#include "engine/core/meta.h"
#include "engine/core/key_codes.h"
#include "engine/core/mouse_codes.h"
#include "engine/impl/math_bitwise.h"
#include "engine/debug/log.h"

#if !defined(CUSTOM_PRECOMPILED_HEADER)
	// #define WIN32_LEAN_AND_MEAN
	// #define NOMINMAX
	#include <Windows.h>
#endif

// @Todo: put all relevant data into the Window object and give out to a user a pointer only
static ivec2 root_window_size;
inline ivec2 get_window_size(HWND window) {
	RECT client_rect;
	GetClientRect(window, &client_rect);
	return {client_rect.right, client_rect.bottom};
}

#include "windows_input_raw.h"
#include "windows_input_keyboard.h"
#include "windows_input_mouse.h"

//
// API implementation
//

static ATOM register_window_class(void);
static HWND create_window(void);
static HWND create_dummy_window(void);

namespace custom {

Window::Window()
	: m_graphics_context(nullptr)
{
	// @Bug: is this error prone to register a window class like that?
	static ATOM const window_atom = register_window_class();

	HWND hwnd = create_window();
	m_handle = (uptr)hwnd;

	#if defined(CUSTOM_FEATURE_RAW_INPUT)
	raw_input_init(hwnd);
	#endif
}

Window::~Window()
{
	delete m_graphics_context;
	m_graphics_context = nullptr;
	DestroyWindow((HWND)m_handle);
	m_handle = 0;
}

void Window::init_context()
{
	if (m_graphics_context) {
		CUSTOM_ASSERT(false, "trying to create a second rendering context");
		return;
	}
	HDC hdc = GetDC((HWND)m_handle);
	m_graphics_context = new Opengl_Context((uptr)hdc);
}

void Window::update()
{
	m_graphics_context->swap_buffers();
}

void Window::set_vsync(s32 value)
{
	m_graphics_context->set_vsync(1);
}

bool Window::is_vsync() const
{
	return m_graphics_context->is_vsync();
}

void Window::set_header(cstring value) const
{
	SetWindowText((HWND)m_handle, value);
}

ivec2 Window::get_size() const
{
	return get_window_size((HWND)m_handle);
}

}

//
// platform implementation
//

static LRESULT CALLBACK window_procedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
static ATOM register_window_class(void) {
	// https://docs.microsoft.com/en-us/windows/win32/api/winuser/ns-winuser-wndclassexa
	WNDCLASSEX window_class = {};
	window_class.cbSize        = sizeof(window_class);
	window_class.style         = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	window_class.lpfnWndProc   = window_procedure;
	window_class.hCursor       = LoadCursor(0, IDC_ARROW);
	window_class.lpszClassName = TEXT(CUSTOM_WINDOW_CLASS_NAME);

	// https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-registerclassexa
	ATOM window_class_atom = RegisterClassEx(&window_class);
	CUSTOM_ASSERT(window_class_atom, "failed to register window class");
	return window_class_atom;
}

static HWND create_window(void) {
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

//
// input
//

#if !defined(CUSTOM_FEATURE_RAW_INPUT)
	static void process_message_raw(HWND hwnd, WPARAM wParam, LPARAM lParam) { /**/ }
#else
	static void raw_input_callback(HWND window, RAWHID const & data) { /*not implemented*/ }
#endif

//
// window procedure callback
// https://docs.microsoft.com/en-us/previous-versions/windows/desktop/legacy/ms633573(v=vs.85)?redirectedfrom=MSDN
//

static LRESULT CALLBACK window_procedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	static HWND root_hwnd = NULL;
	switch (message) {
		// https://docs.microsoft.com/en-us/windows/win32/inputdev/raw-input
		case WM_INPUT: {
			process_message_raw(hwnd, wParam, lParam);
			return 0; // If an application processes this message, it should return zero.
		} break;

		// https://docs.microsoft.com/en-us/windows/win32/inputdev/keyboard-input
		case WM_SYSKEYUP:
		case WM_SYSKEYDOWN:
		case WM_KEYUP:
		case WM_KEYDOWN: {
			process_message_keyboard(hwnd, wParam, lParam);
			return 0; // An application should return zero if it processes this message.
		} break;
		
		// case WM_CHAR: {
		// 	return 0; // An application should return zero if it processes this message.
		// } break;

		case WM_SETFOCUS: {
			// Sent to a window after it has gained the keyboard focus.
			return 0; // An application should return zero if it processes this message.
		} break;
		
		case WM_KILLFOCUS: {
			// Sent to a window immediately before it loses the keyboard focus.
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
			process_message_mouse(hwnd, wParam, lParam);
			return 0; // If an application processes this message, it should return zero.
		} break;

		case WM_MOUSEWHEEL: {
			auto delta = GET_WHEEL_DELTA_WPARAM(wParam);
			// r32 delta = (r32)delta / WHEEL_DELTA;
			return 0; // If an application processes this message, it should return zero.
		} break;
		
		case WM_MOUSEHWHEEL: {
			auto delta = GET_WHEEL_DELTA_WPARAM(wParam);
			// r32 delta = (r32)delta / WHEEL_DELTA;
			return 0; // If an application processes this message, it should return zero.
		} break;

		// https://docs.microsoft.com/en-us/windows/win32/menurc/cursor-notifications
		case WM_SETCURSOR: {
			// Sent to a window if the mouse causes the cursor to move within a window and mouse input is not capture
			WORD hit_test = LOWORD(lParam);
			WORD source_message = HIWORD(lParam);
			// If an application processes this message, it should return TRUE to halt further processing or FALSE to continue.
		} break;

		// https://docs.microsoft.com/en-us/windows/win32/gdi/painting-and-drawing-messages
		case WM_PAINT: {
			// Is sent when the system or another application makes a request to paint a portion of an application's window.
			bool has_update_region = GetUpdateRect(hwnd, NULL, false);
			if (has_update_region) {
				PAINTSTRUCT paint;
				HDC hdc = BeginPaint(hwnd, &paint);
				EndPaint(hwnd, &paint);
			}
			return 0; // An application returns zero if it processes this message.
		} break;

		// https://docs.microsoft.com/en-us/windows/win32/winmsg/window-notifications
		case WM_CREATE: {
			if (!root_hwnd) {
				root_hwnd = hwnd;
				custom::Window::should_close = false;
			}
			return 0; // If an application processes this message, it should return zero to continue creation of the window.
		} break;

		case WM_SIZE: {
			// Sent to a window after its size has changed.
			if (root_hwnd == hwnd) {
				root_window_size = get_window_size(hwnd);
			}
			if (wParam == SIZE_MINIMIZED) {
			}
			else if (wParam == SIZE_MAXIMIZED) {
			}
			else if (wParam == SIZE_RESTORED) {
			}
			return 0; // If an application processes this message, it should return zero.
		} break;

		case WM_SIZING: {
			// Sent to a window that the user is resizing.
			if (root_hwnd == hwnd) {
				root_window_size = get_window_size(hwnd);
			}
			return TRUE; // An application should return TRUE if it processes this message.
		}

		case WM_EXITSIZEMOVE: {
			// Sent one time to a window, after it has exited the moving or sizing modal loop.
			if (root_hwnd == hwnd) {
				root_window_size = get_window_size(hwnd);
			}
			return 0; // An application should return zero if it processes this message.
		} break;

		case WM_CLOSE: {
			// Sent as a signal that a window or an application should terminate.
			if (root_hwnd == hwnd) {
				root_hwnd = NULL;
				custom::Window::should_close = true;
			}
			// DestroyWindow(hwnd); // Go to WM_DESTROY
			return 0; // If an application processes this message, it should return zero.
		} break;

		// @Note: pass the responsibility to the default procedure
		case WM_DESTROY: {
			// Sent when a window is being destroyed. It is sent to the window procedure of the window being destroyed after the window is removed from the screen.
			if (root_hwnd == hwnd) {
				root_hwnd = NULL;
				// @Todo: clean up graphics context
				PostQuitMessage(0); // Go to WM_QUIT
			}
			return 0; // If an application processes this message, it should return zero.
		} break;
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}
