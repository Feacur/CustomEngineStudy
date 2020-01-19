#include "custom_pch.h"
#include "engine/debug/log.h"
#include "engine/platform/platform_system.h"

#if !defined(CUSTOM_PRECOMPILED_HEADER)
	// #define WIN32_LEAN_AND_MEAN
	// #define NOMINMAX
	#include <Windows.h>
	#include <signal.h>
#endif

// https://github.com/Marzac/le3d/blob/master/engine/system_win.cpp

//
// API implementation
//

static bool platform_poll_events(void);
static ULONGLONG platform_get_system_time(void);
static void signal_handler(int value);

namespace custom
{
	System System::s_instance;

	System::System()
		: should_close(false)
	{
		signal(SIGABRT, signal_handler);
		// signal(SIGFPE, SIG_DFL);
		// signal(SIGILL, SIG_DFL);
		signal(SIGINT, signal_handler);
		// signal(SIGSEGV, SIG_DFL);
		signal(SIGTERM, signal_handler);
	}

	System::~System() = default;

	void System::update()
	{
		bool quit_request = platform_poll_events();
		if (quit_request) { should_close = true; }
	}

	u64 System::get_system_time()
	{
		ULONGLONG system_time = platform_get_system_time();
		return (u64)system_time;
	}
}

//
// platform implementation
//

static bool platform_poll_events(void) {
	bool quit_request = false;
	MSG message = {};
	while (PeekMessage(&message, 0, 0, 0, PM_REMOVE)) {
		if (message.message == WM_QUIT) {
			// https://docs.microsoft.com/en-us/windows/win32/winmsg/wm-quit
			// Indicates a request to terminate an application.
			// This message does not have a return value because it causes the message loop to terminate before the message is sent to the application's window procedure.
			// The WM_QUIT message is not associated with a window and therefore will never be received through a window's window procedure.
			// * wParam - The exit code given in the PostQuitMessage function.
			quit_request = true;
			continue;
		}
		TranslateMessage(&message);
		DispatchMessage(&message);
	}
	return quit_request;
}

static ULONGLONG platform_get_system_time(void) {
	FILETIME file_time;
	GetSystemTimeAsFileTime(&file_time);

	ULARGE_INTEGER ularge_time;
	ularge_time.LowPart = file_time.dwLowDateTime;
	ularge_time.HighPart = file_time.dwHighDateTime;

	return ularge_time.QuadPart;
}

// http://www.cplusplus.com/reference/csignal/signal/
static void signal_handler(int value) {
	switch (value) {
		case SIGABRT: CUSTOM_ERROR("Abort signal");             break; // Abnormal termination, such as is initiated by the abort function.
		// case SIGFPE:  CUSTOM_ERROR("Floating-Point Exception"); break; // Erroneous arithmetic operation, such as zero divide or an operation resulting in overflow (not necessarily with a floating-point operation).
		// case SIGILL:  CUSTOM_ERROR("Illegal Instruction");      break; // Invalid function image, such as an illegal instruction. This is generally due to a corruption in the code or to an attempt to execute data.
		case SIGINT:  CUSTOM_ERROR("Interrupt signal");         break; // Interactive attention signal. Generally generated by the application user.
		// case SIGSEGV: CUSTOM_ERROR("Segmentation Violation");   break; // Invalid access to storage: When a program tries to read or write outside the memory it has allocated.
		case SIGTERM: CUSTOM_ERROR("Terminate signal");         break; // Termination request sent to program.
		default:      CUSTOM_ERROR("Unknown signal");           break; // ?
	}
	custom::System::get().should_close = true;
}

// GetSystemMetrics with SM_CXSCREEN and SM_CYSCREEN
// ivec2 monitor_size = {
// 	(s32)GetDeviceCaps(device_context, HORZRES),
// 	(s32)GetDeviceCaps(device_context, VERTRES)
// };
