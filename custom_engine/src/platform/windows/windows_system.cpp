#include "custom_pch.h"
#include "engine/api/system.h"
#include "engine/core/code.h"
#include "engine/debug/log.h"

#if !defined(CUSTOM_PRECOMPILED_HEADER)
	// #define WIN32_LEAN_AND_MEAN
	// #define NOMINMAX
	#include <Windows.h>
	#include <signal.h>
#endif

#if defined(CUSTOM_SHARED_LIBRARY)
	#pragma message("engine is being built as a shared library")
	#pragma message("and won't export performance variables")
#else
	extern "C" { // @Note: use discrete GPU by default
		// http://developer.download.nvidia.com/devzone/devcenter/gamegraphics/files/OptimusRenderingPolicies.pdf
		// Global Variable NvOptimusEnablement (new in Driver Release 302)
		// Starting with the Release 302 drivers, application developers can direct the Optimus driver at runtime to use the High Performance Graphics to render any application–even those applications for which there is no existing application profile. They can do this by exporting a global variable named NvOptimusEnablement. The Optimus driver looks for the existence and value of the export. Only the LSB of the DWORD matters at this time. Avalue of 0x00000001 indicates that rendering should be performed using High Performance Graphics. A value of 0x00000000 indicates that this method should beignored.
		__declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001UL;

		// https://community.amd.com/thread/169965
		// https://community.amd.com/thread/223376
		// https://gpuopen.com/amdpowerxpressrequesthighperformance/
		// Summary
		// Many Gaming and workstation laptops are available with both (1) integrated power saving and (2) discrete high performance graphics devices. Unfortunately, 3D intensive application performance may suffer greatly if the best graphics device is not selected. For example, a game may run at 30 Frames Per Second (FPS) on the integrated GPU rather than the 60 FPS the discrete GPU would enable. As a developer you can easily fix this problem by adding only one line to your executable’s source code:
		__declspec(dllexport) DWORD AmdPowerXpressRequestHighPerformance = 0x00000001UL;
		// Yes, it’s that easy. This line will ensure that the high-performance graphics device is chosen when running your application.
	}
#endif

// https://github.com/Marzac/le3d/blob/master/engine/system_win.cpp

//
// API implementation
//

static bool platform_poll_events(void);
static ULONGLONG platform_get_system_time(void);
static void signal_handler(int value);

namespace custom {

extern void globals_init();

void system_init() {
	globals_init();

	signal(SIGABRT, signal_handler);
	// signal(SIGFPE, SIG_DFL);
	// signal(SIGILL, SIG_DFL);
	signal(SIGINT, signal_handler);
	// signal(SIGSEGV, SIG_DFL);
	signal(SIGTERM, signal_handler);
}

void system_update()
{
	bool quit_request = platform_poll_events();
	if (quit_request) { system.should_close = true; }
}

u64 system_get_time()
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
	// GetSystemTimePreciseAsFileTime(&file_time);

	ULARGE_INTEGER ularge_time;
	ularge_time.LowPart = file_time.dwLowDateTime;
	ularge_time.HighPart = file_time.dwHighDateTime;

	return ularge_time.QuadPart;
}

// http://www.cplusplus.com/reference/csignal/signal/
static void signal_handler(int value) {
	switch (value) {
		case SIGABRT: CUSTOM_MESSAGE("Abort signal");             break; // Abnormal termination, such as is initiated by the abort function.
		// case SIGFPE:  CUSTOM_MESSAGE("Floating-Point Exception"); break; // Erroneous arithmetic operation, such as zero divide or an operation resulting in overflow (not necessarily with a floating-point operation).
		// case SIGILL:  CUSTOM_MESSAGE("Illegal Instruction");      break; // Invalid function image, such as an illegal instruction. This is generally due to a corruption in the code or to an attempt to execute data.
		case SIGINT:  CUSTOM_MESSAGE("Interrupt signal");         break; // Interactive attention signal. Generally generated by the application user.
		// case SIGSEGV: CUSTOM_MESSAGE("Segmentation Violation");   break; // Invalid access to storage: When a program tries to read or write outside the memory it has allocated.
		case SIGTERM: CUSTOM_MESSAGE("Terminate signal");         break; // Termination request sent to program.
		default:      CUSTOM_MESSAGE("Unknown signal");           break; // ?
	}
	custom::system.should_close = true;
}

// GetSystemMetrics with SM_CXSCREEN and SM_CYSCREEN
// ivec2 monitor_size = {
// 	(s32)GetDeviceCaps(device_context, HORZRES),
// 	(s32)GetDeviceCaps(device_context, VERTRES)
// };
