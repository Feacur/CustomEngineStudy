#include "custom_pch.h"
#include "engine/api/platform/system.h"
#include "engine/core/code.h"
#include "engine/debug/log.h"

#if !defined(CUSTOM_PRECOMPILED_HEADER)
	#include <signal.h>
	#include <Windows.h>
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

// code page 65001: Unicode (UTF-8)
// code page 1250:  ANSI Central European; Central European (Windows)
// code page 1251:  ANSI Cyrillic; Cyrillic (Windows)
// code page 1252:  ANSI Latin 1; Western European (Windows)

//
// API implementation
//

static void platform_setup_console();
static bool platform_poll_events(void);
static ULONGLONG platform_get_system_time(void);
static void signal_handler(int value);

namespace custom {
namespace globals {
extern void init(void);
}}

namespace custom {
namespace system {

void init(void) {
	globals::init();
	platform_setup_console();

	signal(SIGABRT, signal_handler);
	// signal(SIGFPE, SIG_DFL);
	// signal(SIGILL, SIG_DFL);
	signal(SIGINT, signal_handler);
	// signal(SIGSEGV, SIG_DFL);
	signal(SIGTERM, signal_handler);
}

void update(void)
{
	bool quit_request = platform_poll_events();
	if (quit_request) { should_close = true; }
}

u64 get_time(void)
{
	ULONGLONG system_time = platform_get_system_time();
	return (u64)system_time;
}

}}

//
// platform implementation
//

#if !defined(CUSTOM_SHIPPING)
static void PLATFORM_WINMAIN_SHOW_CONSOLE() {
	if (AllocConsole()) {
		FILE * file_stdout = NULL;
		freopen_s(&file_stdout, "CONOUT$", "w", stdout);
		
		FILE * file_stdin = NULL;
		freopen_s(&file_stdin, "CONIN$", "r", stdin);
	}
	// FreeConsole();
}

static void platform_enable_virtual_terminal_processing() {
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	if (handle == INVALID_HANDLE_VALUE) {
		CUSTOM_WARNING("can't get std out handle");
		return;
	}
	DWORD mode = 0;
	if (!GetConsoleMode(handle, &mode)) {
		CUSTOM_WARNING("can't get std out mode");
		return;
	}
	if (!SetConsoleMode(handle, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING)) {
		CUSTOM_WARNING("can't enable virtual terminal processing for std out");
	}
}

static void platform_setup_console() {
	platform_enable_virtual_terminal_processing();
	SetConsoleOutputCP(65001);
}
#else
#define PLATFORM_WINMAIN_SHOW_CONSOLE() (void)0
static void platform_setup_console() {
	if (HWND console = GetConsoleWindow()) {
		ShowWindow(console, SW_HIDE);
		// ShowWindow(console, SW_SHOW);
	}
}
#endif

static bool platform_poll_events(void) {
	bool quit_request = false;
	MSG message; // = {};
	// ZeroMemory(&message, sizeof(message));
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
		case SIGABRT: CUSTOM_TRACE("Abort signal");             break; // Abnormal termination, such as is initiated by the abort function.
		case SIGFPE:  CUSTOM_TRACE("Floating-Point Exception"); break; // Erroneous arithmetic operation, such as zero divide or an operation resulting in overflow (not necessarily with a floating-point operation).
		case SIGILL:  CUSTOM_TRACE("Illegal Instruction");      break; // Invalid function image, such as an illegal instruction. This is generally due to a corruption in the code or to an attempt to execute data.
		case SIGINT:  CUSTOM_TRACE("Interrupt signal");         break; // Interactive attention signal. Generally generated by the application user.
		case SIGSEGV: CUSTOM_TRACE("Segmentation Violation");   break; // Invalid access to storage: When a program tries to read or write outside the memory it has allocated.
		case SIGTERM: CUSTOM_TRACE("Terminate signal");         break; // Termination request sent to program.
		default:      CUSTOM_TRACE("Unknown signal");           break; // ?
	}
	custom::system::should_close = true;
}

// GetSystemMetrics with SM_CXSCREEN and SM_CYSCREEN
// ivec2 monitor_size = {
// 	(s32)GetDeviceCaps(device_context, HORZRES),
// 	(s32)GetDeviceCaps(device_context, VERTRES)
// };

//
//
//

#include <stdlib.h>

// https://docs.microsoft.com/en-us/cpp/c-runtime-library/argc-argv-wargv
// https://docs.microsoft.com/en-us/windows/win32/desktop-programming
// https://docs.microsoft.com/en-us/windows/win32/dlls/dllmain
// https://docs.microsoft.com/en-us/windows/win32/learnwin32/winmain--the-application-entry-point

extern int main(int argc, char * argv[]);
int WINAPI WinMain(
	HINSTANCE hInstance,     // is something called a "handle to an instance" or "handle to a module." The operating system uses this value to identify the executable (EXE) when it is loaded in memory. The instance handle is needed for certain Windows functions—for example, to load icons or bitmaps.
	HINSTANCE hPrevInstance, // has no meaning. It was used in 16-bit Windows, but is now always zero.
	PSTR      pCmdLine,      // contains the command-line arguments as an ANSI string.
	int       nCmdShow       // is a flag that says whether the main application window will be minimized, maximized, or shown normally.
) {
	PLATFORM_WINMAIN_SHOW_CONSOLE();
	return main(__argc, __argv);
}
