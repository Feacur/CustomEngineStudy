#include "windows_window.h"

#include "engine/debug/log.h"

#include <Windows.h>

using namespace custom;

static LPTSTR const window_class_name = TEXT("custom engine");
static LPTSTR const window_title = TEXT("");

static LRESULT CALLBACK window_procedure(HWND window, UINT message, WPARAM wParam, LPARAM lParam);

#pragma warning(push)
#pragma warning(disable: 4505)

//
// API
//

static u64 register_window_class() {
	// https://docs.microsoft.com/en-us/windows/win32/api/winuser/ns-winuser-wndclassexa
	WNDCLASSEX window_class = {};
	window_class.cbSize        = sizeof(window_class);
	window_class.style         = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	window_class.lpfnWndProc   = window_procedure;
	window_class.hCursor       = LoadCursor(0, IDC_ARROW);
	window_class.lpszClassName = window_class_name;

	// https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-registerclassexa
	ATOM window_class_atom = RegisterClassEx(&window_class);
	CUSTOM_ASSERT(window_class_atom, "didn't register window class");
	return (u64)window_class_atom;
}

static u64 create_window() {
	DWORD     dwExStyle  = WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR;
	DWORD     dwStyle    = WS_OVERLAPPEDWINDOW | WS_VISIBLE;
	HWND      hWndParent = HWND_DESKTOP;
	HMENU     hMenu      = NULL;
	HINSTANCE hInstance  = NULL;
	LPVOID    lpParam    = NULL;

	// https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-createwindowexa
	HWND window_handle = CreateWindowEx(
		dwExStyle,
		window_class_name, window_title,
		dwStyle,
		// i32 X, Y, nWidth, nHeight
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		hWndParent, hMenu, hInstance, lpParam
	);
	CUSTOM_ASSERT(window_handle, "didn't create window");
	return (u64)window_handle;
}

static void destroy_window(u64 window_handle) {
	DestroyWindow((HWND)window_handle);
}

//
// input
//

static void process_message_raw(HWND window_handle, LPARAM lParam) { /**/ }
static void process_message_keyboard(HWND window_handle, WPARAM wParam, LPARAM lParam) { /**/ }
static void process_message_mouse(HWND window_handle, WPARAM wParam, LPARAM lParam) { /**/ }

//
// window procedure callback
// https://docs.microsoft.com/en-us/previous-versions/windows/desktop/legacy/ms633573(v=vs.85)?redirectedfrom=MSDN
//

static LRESULT CALLBACK window_procedure(HWND window_handle, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
		// https://docs.microsoft.com/en-us/windows/win32/inputdev/raw-input
		case WM_INPUT: {
			process_message_raw(window_handle, lParam);
			return 0; // If an application processes this message, it should return zero.
		} break;

		// https://docs.microsoft.com/en-us/windows/win32/inputdev/keyboard-input
		case WM_SYSKEYUP:
		case WM_SYSKEYDOWN:
		case WM_KEYUP:
		case WM_KEYDOWN: {
			process_message_keyboard(window_handle, wParam, lParam);
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
		case WM_RBUTTONUP: {
			process_message_mouse(window_handle, wParam, lParam);
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
			bool has_update_region = GetUpdateRect(window_handle, NULL, false);
			if (has_update_region) {
				PAINTSTRUCT paint;
				HDC device_context = BeginPaint(window_handle, &paint);
				EndPaint(window_handle, &paint);
			}
			return 0; // An application returns zero if it processes this message.
		} break;

		// https://docs.microsoft.com/en-us/windows/win32/winmsg/window-notifications
		case WM_SIZE: {
			// Sent to a window after its size has changed.
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
			return TRUE; // An application should return TRUE if it processes this message.
		}

		case WM_EXITSIZEMOVE: {
			// Sent one time to a window, after it has exited the moving or sizing modal loop.
			return 0; // An application should return zero if it processes this message.
		} break;

		case WM_CLOSE: {
			// Sent as a signal that a window or an application should terminate.
			DestroyWindow(window_handle); // Go to WM_DESTROY
			return 0; // If an application processes this message, it should return zero.
		} break;

		case WM_DESTROY: {
			// Sent when a window is being destroyed. It is sent to the window procedure of the window being destroyed after the window is removed from the screen.
			PostQuitMessage(0); // Go to WM_QUIT
			return 0; // If an application processes this message, it should return zero.
		} break;
	}

	return DefWindowProc(window_handle, message, wParam, lParam);
}

#pragma warning(pop)
