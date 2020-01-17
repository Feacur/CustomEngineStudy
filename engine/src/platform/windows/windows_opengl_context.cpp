#include "custom_pch.h"
#include "windows_opengl_context.h"
#include "engine/debug/log.h"

#if !defined(CUSTOM_PRECOMPILED_HEADER)
	#include <Windows.h>
	#include <glad/glad.h>
#endif

static void log_last_error() {
	auto error = GetLastError();
	if (!error) { return; }

	LPTSTR messageBuffer = NULL;
	size_t size = FormatMessage(
		FORMAT_MESSAGE_FROM_SYSTEM
		| FORMAT_MESSAGE_ALLOCATE_BUFFER
		| FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, error,
		MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT),
		(LPTSTR)&messageBuffer, 0,
		NULL
	);

	if (size) {
		printf("0x%x: %s\n", error, messageBuffer);
		LocalFree(messageBuffer);
		return;
	}

	printf("0x%x\n", error);
}

constexpr inline bool bits_are_set(DWORD container, DWORD bits) {
	return (container & bits) == bits;
}

static void platform_init(HDC hdc);
static void platform_shutdown();

//
// API implementation
//

namespace custom
{
	void Opengl_Context::init(u64 graphics)
	{
		platform_init((HDC)graphics);
	}

	void Opengl_Context::shutdown()
	{
		platform_shutdown();
	}
}

//
// platform implementation
//

// https://docs.microsoft.com/en-us/windows/win32/api/wingdi/
typedef HGLRC (WINAPI CreateContext_func)(HDC hdc);
typedef BOOL  (WINAPI DeleteContext_func)(HGLRC hrc);
typedef HGLRC (WINAPI GetCurrentContext_func)(VOID);
typedef HDC   (WINAPI GetCurrentDC_func)(VOID);
typedef PROC  (WINAPI GetProcAddress_func)(LPCSTR name);
typedef BOOL  (WINAPI MakeCurrent_func)(HDC hdc, HGLRC hrc);
typedef BOOL  (WINAPI ShareLists_func)(HGLRC target, HGLRC source);

// https://www.khronos.org/registry/OpenGL/extensions/EXT
typedef cstring (WINAPI GetExtensionsStringEXT_func)(void);
typedef BOOL    (WINAPI SwapIntervalEXT_func)(void);

// https://www.khronos.org/registry/OpenGL/extensions/ARB/
typedef cstring (WINAPI GetExtensionsStringARB_func)(HDC hdc);
typedef HGLRC   (WINAPI CreateContextAttribsARB_func)(HDC hdc, HGLRC hrc, const int * attributes);
typedef BOOL    (WINAPI GetPixelFormatAttribivARB_func)(void);
typedef BOOL    (WINAPI ChoosePixelFormatARB_func)(HDC hdc, const int * attributes, const FLOAT * attributes_f, UINT formats_limit, int * pixel_format, UINT * formats_count);

namespace custom
{
	struct Wgl_Context
	{
		HINSTANCE instance;
		CreateContext_func     * CreateContext;
		DeleteContext_func     * DeleteContext;
		GetCurrentContext_func * GetCurrentContext;
		GetCurrentDC_func      * GetCurrentDC;
		GetProcAddress_func    * GetProcAddress;
		MakeCurrent_func       * MakeCurrent;
		ShareLists_func        * ShareLists;

		GetExtensionsStringEXT_func    * GetExtensionsStringEXT;
		SwapIntervalEXT_func           * SwapIntervalEXT;

		GetExtensionsStringARB_func    * GetExtensionsStringARB;
		CreateContextAttribsARB_func   * CreateContextAttribsARB;
		GetPixelFormatAttribivARB_func * GetPixelFormatAttribivARB;
		ChoosePixelFormatARB_func      * ChoosePixelFormatARB;
	};
}
static custom::Wgl_Context wgl;

#define LOAD_OPENGL_FUNCTION(name) wgl.name = (name##_func *)GetProcAddress(wgl.instance, "wgl" #name)
static void platform_load_opengl() {
	LOAD_OPENGL_FUNCTION(CreateContext);
	LOAD_OPENGL_FUNCTION(DeleteContext);
	LOAD_OPENGL_FUNCTION(GetCurrentContext);
	LOAD_OPENGL_FUNCTION(GetCurrentDC);
	LOAD_OPENGL_FUNCTION(GetProcAddress);
	LOAD_OPENGL_FUNCTION(MakeCurrent);
	LOAD_OPENGL_FUNCTION(ShareLists);
}
#undef LOAD_OPENGL_FUNCTION

#define LOAD_EXTENSION_FUNCTION(name) wgl.name = (name##_func *)wgl.GetProcAddress("wgl" #name)
static void platform_load_extensions() {
	LOAD_EXTENSION_FUNCTION(GetExtensionsStringEXT);
	LOAD_EXTENSION_FUNCTION(SwapIntervalEXT);
	LOAD_EXTENSION_FUNCTION(GetExtensionsStringARB);
	LOAD_EXTENSION_FUNCTION(CreateContextAttribsARB);
	LOAD_EXTENSION_FUNCTION(GetPixelFormatAttribivARB);
	LOAD_EXTENSION_FUNCTION(ChoosePixelFormatARB);
}
#undef LOAD_EXTENSION_FUNCTION

static void platform_load_ext_arb(HDC hdc) {
	PIXELFORMATDESCRIPTOR pfd = {};
	pfd.nSize        = sizeof(pfd);
	pfd.nVersion     = 1;
	pfd.dwFlags      = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType   = PFD_TYPE_RGBA;
	pfd.cColorBits   = 8 * 3;
	// pfd.cDepthBits   = 8 * 3;
	// pfd.cStencilBits = 8 * 1;

	if (!SetPixelFormat(hdc, ChoosePixelFormat(hdc, &pfd), &pfd)) {
		CUSTOM_ASSERT(false, "can't set dummy pixel format");
		return;
	}

	HGLRC dummy_hrc = wgl.CreateContext(hdc);
	if (!dummy_hrc) {
		CUSTOM_ASSERT(false, "can't create dummy rendering context");
		return;
	}

	HDC current_hdc = wgl.GetCurrentDC();
	HGLRC current_hrc = wgl.GetCurrentContext();

	if (!wgl.MakeCurrent(hdc, dummy_hrc)) {
		CUSTOM_ASSERT(false, "can't make dummy rendering context the current one");
	}
	else {
		platform_load_extensions();
	}

	wgl.MakeCurrent(current_hdc, current_hrc);
	wgl.DeleteContext(dummy_hrc);
}

static void * get_proc_address(cstring name) {
	CUSTOM_ASSERT(name, "null GL procedure name");
	void * address = wgl.GetProcAddress(name);
	if (!address) {
		address = GetProcAddress(wgl.instance, name);
	}
	return address;
}

#define WGL_NUMBER_PIXEL_FORMATS_ARB            0x2000
#define WGL_DRAW_TO_WINDOW_ARB                  0x2001
#define WGL_DRAW_TO_BITMAP_ARB                  0x2002
#define WGL_ACCELERATION_ARB                    0x2003
#define WGL_NEED_PALETTE_ARB                    0x2004
#define WGL_NEED_SYSTEM_PALETTE_ARB             0x2005
#define WGL_SWAP_LAYER_BUFFERS_ARB              0x2006
#define WGL_SWAP_METHOD_ARB                     0x2007
#define WGL_NUMBER_OVERLAYS_ARB                 0x2008
#define WGL_NUMBER_UNDERLAYS_ARB                0x2009
#define WGL_TRANSPARENT_ARB                     0x200A
#define WGL_TRANSPARENT_RED_VALUE_ARB           0x2037
#define WGL_TRANSPARENT_GREEN_VALUE_ARB         0x2038
#define WGL_TRANSPARENT_BLUE_VALUE_ARB          0x2039
#define WGL_TRANSPARENT_ALPHA_VALUE_ARB         0x203A
#define WGL_TRANSPARENT_INDEX_VALUE_ARB         0x203B
#define WGL_SHARE_DEPTH_ARB                     0x200C
#define WGL_SHARE_STENCIL_ARB                   0x200D
#define WGL_SHARE_ACCUM_ARB                     0x200E
#define WGL_SUPPORT_GDI_ARB                     0x200F
#define WGL_SUPPORT_OPENGL_ARB                  0x2010
#define WGL_DOUBLE_BUFFER_ARB                   0x2011
#define WGL_STEREO_ARB                          0x2012
#define WGL_PIXEL_TYPE_ARB                      0x2013
#define WGL_COLOR_BITS_ARB                      0x2014
#define WGL_RED_BITS_ARB                        0x2015
#define WGL_RED_SHIFT_ARB                       0x2016
#define WGL_GREEN_BITS_ARB                      0x2017
#define WGL_GREEN_SHIFT_ARB                     0x2018
#define WGL_BLUE_BITS_ARB                       0x2019
#define WGL_BLUE_SHIFT_ARB                      0x201A
#define WGL_ALPHA_BITS_ARB                      0x201B
#define WGL_ALPHA_SHIFT_ARB                     0x201C
#define WGL_ACCUM_BITS_ARB                      0x201D
#define WGL_ACCUM_RED_BITS_ARB                  0x201E
#define WGL_ACCUM_GREEN_BITS_ARB                0x201F
#define WGL_ACCUM_BLUE_BITS_ARB                 0x2020
#define WGL_ACCUM_ALPHA_BITS_ARB                0x2021
#define WGL_DEPTH_BITS_ARB                      0x2022
#define WGL_STENCIL_BITS_ARB                    0x2023
#define WGL_AUX_BUFFERS_ARB                     0x2024

#define WGL_NO_ACCELERATION_ARB                 0x2025
#define WGL_GENERIC_ACCELERATION_ARB            0x2026
#define WGL_FULL_ACCELERATION_ARB               0x2027

#define WGL_SWAP_EXCHANGE_ARB                   0x2028
#define WGL_SWAP_COPY_ARB                       0x2029
#define WGL_SWAP_UNDEFINED_ARB                  0x202A

#define WGL_TYPE_RGBA_ARB                       0x202B
#define WGL_TYPE_COLORINDEX_ARB                 0x202C

static void platform_init(HDC hdc) {
	HINSTANCE opengl_handle = LoadLibrary("opengl32.dll");
	if (!opengl_handle) {
		CUSTOM_ASSERT(false, "can't load opengl32.dll");
		return;
	}

	wgl.instance = opengl_handle;
	platform_load_opengl();
	platform_load_ext_arb(hdc);

	const int attributes[] = {
		WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
		WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
		WGL_DOUBLE_BUFFER_ARB,  GL_TRUE,
		WGL_PIXEL_TYPE_ARB,     WGL_TYPE_RGBA_ARB,
		WGL_COLOR_BITS_ARB,     32,
		WGL_DEPTH_BITS_ARB,     24,
		WGL_STENCIL_BITS_ARB,    8,
		0, // End
	};

	int pixel_format;
	UINT formats_count;
	wgl.ChoosePixelFormatARB(hdc, attributes, NULL, 1, &pixel_format, &formats_count);

	PIXELFORMATDESCRIPTOR pfd;
	if (!DescribePixelFormat(hdc, pixel_format, sizeof(pfd), &pfd)) {
		CUSTOM_ASSERT(false, "can't describe pixel format");
		return;
	}

	if (!SetPixelFormat(hdc, pixel_format, &pfd)) {
		CUSTOM_ASSERT(false, "can't set pixel format");
		return;
	}

	HGLRC share = NULL;
	HGLRC hrc = wgl.CreateContextAttribsARB(hdc, share, attributes);
	if (!hrc) {
		log_last_error();
		CUSTOM_ASSERT(false, "can't create rendering context");
		return;
	}

	if (!wgl.MakeCurrent(hdc, hrc)) {
		CUSTOM_ASSERT(false, "can't make rendering context the current one");
		return;
	}
	
	int glad_status = gladLoadGLLoader((GLADloadproc)get_proc_address);
	CUSTOM_ASSERT(glad_status, "failed to initialize glad");
}

static void platform_shutdown() {
	FreeLibrary(wgl.instance);
}

// CUSTOM_INFO("vendor '%s'; renderer '%s'; version '%s'", glGetString(GL_VENDOR), glGetString(GL_RENDERER), glGetString(GL_VERSION));

// wglSwapIntervalEXT = (wglSwapIntervalEXT_func *)wglGetProcAddress("wglSwapIntervalEXT");
// if (wglSwapIntervalEXT) {
// 	wglSwapIntervalEXT(1);
// }
