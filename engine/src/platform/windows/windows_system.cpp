#include "windows_window.h"

#include "engine/debug/log.h"

#include <Windows.h>

using namespace custom;

#pragma warning(push)
#pragma warning(disable: 4505)

//
// API
//

static void update() {
	MSG message = {};
	while(PeekMessage(&message, 0, 0, 0, PM_REMOVE)) {
		if (message.message == WM_QUIT) {
			// Indicates a request to terminate an application.
			break;
		}
		TranslateMessage(&message);
		DispatchMessage(&message);
	}
}

#pragma warning(pop)
