#include "custom_pch.h"
#include "windows_opengl_context.h"
#include "engine/debug/log.h"

#if !defined(CUSTOM_PRECOMPILED_HEADER)
	#include <Windows.h>
#endif

constexpr inline bool bits_are_set(DWORD container, DWORD bits) {
	return (container & bits) == bits;
}

static void platform_init(HDC device_context);

//
// API implementation
//

namespace custom
{
	void Opengl_Context::init(u64 graphics)
	{
		platform_init((HDC)graphics);
	}
}

//
// platform implementation
//

static void platform_verify_pixel_format(HDC device_context) {
	int active_format = GetPixelFormat(device_context);
	if (!active_format) {
		CUSTOM_ASSERT(false, "no active pixel format");
		return;
	}

	PIXELFORMATDESCRIPTOR pfd;
	int maximum_format = DescribePixelFormat(device_context, active_format, sizeof(pfd), &pfd);
	if (!maximum_format) {
		CUSTOM_ASSERT(false, "can't describe pixel format");
		return;
	}

	if (!bits_are_set(pfd.dwFlags, PFD_SUPPORT_OPENGL)) {
		CUSTOM_ASSERT(false, "pixel format doesn't support OpenGL");
		return;
	}
}

static void platform_init(HDC device_context) {
	PIXELFORMATDESCRIPTOR pfd = {};
	pfd.nSize        = sizeof(pfd);
	pfd.nVersion     = 1;
	pfd.dwFlags      = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType   = PFD_TYPE_RGBA;
	pfd.cColorBits   = (BYTE)(8 * 4);
	// pfd.cDepthBits   = (BYTE)(8 * 3);
	// pfd.cStencilBits = (BYTE)(8 * 1);
	
	int format = ChoosePixelFormat(device_context, &pfd);
	if (!format) {
		CUSTOM_ASSERT(false, "can't choose pixel format");
		return;
	}
	
	if (!SetPixelFormat(device_context, format, &pfd)) {
		CUSTOM_ASSERT(false, "can't set pixel format");
		return;
	}

	platform_verify_pixel_format(device_context);

	HGLRC rendering_context = wglCreateContext(device_context);
	if (!rendering_context) {
		CUSTOM_ASSERT(false, "can't create rendering context");
		return;
	}

	if (!wglMakeCurrent(device_context, rendering_context)) {
		CUSTOM_ASSERT(false, "can't select rendering context");
		return;
	}

	// CUSTOM_INFO("vendor '%s'; renderer '%s'; version '%s'", glGetString(GL_VENDOR), glGetString(GL_RENDERER), glGetString(GL_VERSION));

	// wglSwapIntervalEXT = (wglSwapIntervalEXT_func *)wglGetProcAddress("wglSwapIntervalEXT");
	// if (wglSwapIntervalEXT) {
	// 	wglSwapIntervalEXT(1);
	// }
}
