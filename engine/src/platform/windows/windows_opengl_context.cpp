#include "custom_pch.h"
#include "windows_opengl_context.h"
#include "engine/debug/log.h"

#if !defined(CUSTOM_PRECOMPILED_HEADER)
	#include <Windows.h>
	#include <glad/glad.h>
#endif

// https://www.khronos.org/registry/OpenGL/api/GL
// @Note: see "wgl_reference/wgl.h", "wgl_reference/wglext.h"

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
typedef HGLRC (WINAPI CreateContext_func)(HDC hDc);
typedef BOOL  (WINAPI DeleteContext_func)(HGLRC oldContext);
typedef HGLRC (WINAPI GetCurrentContext_func)(void);
typedef HDC   (WINAPI GetCurrentDC_func)(void);
typedef PROC  (WINAPI GetProcAddress_func)(LPCSTR lpszProc);
typedef BOOL  (WINAPI MakeCurrent_func)(HDC hDc, HGLRC newContext);
typedef BOOL  (WINAPI ShareLists_func)(HGLRC hrcSrvShare, HGLRC hrcSrvSource);

// https://www.khronos.org/registry/OpenGL/extensions/EXT
typedef cstring (WINAPI GetExtensionsStringEXT_func)(void);
typedef BOOL    (WINAPI SwapIntervalEXT_func)(void);

// https://www.khronos.org/registry/OpenGL/extensions/ARB/
typedef cstring (WINAPI GetExtensionsStringARB_func)(HDC hdc);
typedef HGLRC   (WINAPI CreateContextAttribsARB_func)(HDC hDC, HGLRC hShareContext, const int *attribList);
typedef BOOL    (WINAPI GetPixelFormatAttribivARB_func)(HDC hdc, int iPixelFormat, int iLayerPlane, UINT nAttributes, const int *piAttributes, int *piValues);

typedef BOOL    (WINAPI ChoosePixelFormatARB_func)(HDC hdc, const int *piAttribIList, const FLOAT *pfAttribFList, UINT nMaxFormats, int *piFormats, UINT *nNumFormats);

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

		ChoosePixelFormatARB_func * ChoosePixelFormatARB;
	};
}
static custom::Wgl_Context wgl;

#define LOAD_OPENGL_FUNCTION(name) wgl.name = (name##_func *)GetProcAddress(wgl.instance, "wgl" #name)
static void platform_init_opengl() {
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
static void platform_init_extensions() {
	LOAD_EXTENSION_FUNCTION(GetExtensionsStringEXT);
	LOAD_EXTENSION_FUNCTION(SwapIntervalEXT);
	LOAD_EXTENSION_FUNCTION(GetExtensionsStringARB);
	LOAD_EXTENSION_FUNCTION(CreateContextAttribsARB);
	LOAD_EXTENSION_FUNCTION(GetPixelFormatAttribivARB);
	LOAD_EXTENSION_FUNCTION(ChoosePixelFormatARB);
}
#undef LOAD_EXTENSION_FUNCTION

static void platform_init_ext_arb(HDC hdc) {
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
		platform_init_extensions();
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

#define WGL_NUMBER_PIXEL_FORMATS_ARB      0x2000
#define WGL_CONTEXT_MAJOR_VERSION_ARB     0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB     0x2092
#define WGL_CONTEXT_FLAGS_ARB             0x2094

int choose_pixel_format(HDC hdc) {
	// https://www.khronos.org/registry/OpenGL/extensions/ARB/WGL_ARB_pixel_format.txt
	const int attrib = WGL_NUMBER_PIXEL_FORMATS_ARB;
	// The number of pixel formats for the device context.
	// The <iLayerPlane> and <iPixelFormat> parameters are ignored
	int nativeCount;

	if (!wgl.GetPixelFormatAttribivARB(hdc, NULL, NULL, 1, &attrib, &nativeCount))
	{
	}

	return 0;
}

static void platform_init_create(HDC hdc) {
	int const attributes[] = {
		WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
		WGL_CONTEXT_MINOR_VERSION_ARB, 1,
		WGL_CONTEXT_FLAGS_ARB, 0,
		0
	};

	int pixel_format;
	UINT formats_count;
	wgl.ChoosePixelFormatARB(hdc, attributes, NULL, 1, &pixel_format, &formats_count);

	PIXELFORMATDESCRIPTOR pfd;
	if (!DescribePixelFormat(hdc, pixel_format, sizeof(pfd), &pfd)) {
		log_last_error();
		CUSTOM_ASSERT(false, "can't describe pixel format");
		return;
	}

	if (!SetPixelFormat(hdc, pixel_format, &pfd)) {
		log_last_error();
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
		log_last_error();
		CUSTOM_ASSERT(false, "can't make rendering context the current one");
		return;
	}
	
	int glad_status = gladLoadGLLoader((GLADloadproc)get_proc_address);
	CUSTOM_ASSERT(glad_status, "failed to initialize glad");
}

static void platform_init(HDC hdc) {
	HINSTANCE opengl_handle = LoadLibrary("opengl32.dll");
	if (!opengl_handle) {
		CUSTOM_ASSERT(false, "can't load opengl32.dll");
		return;
	}

	wgl.instance = opengl_handle;
	platform_init_opengl();
	platform_init_ext_arb(hdc);
	platform_init_create(hdc);
	
}

static void platform_shutdown() {
	FreeLibrary(wgl.instance);
}

// CUSTOM_INFO("vendor '%s'; renderer '%s'; version '%s'", glGetString(GL_VENDOR), glGetString(GL_RENDERER), glGetString(GL_VERSION));

// wglSwapIntervalEXT = (wglSwapIntervalEXT_func *)wglGetProcAddress("wglSwapIntervalEXT");
// if (wglSwapIntervalEXT) {
// 	wglSwapIntervalEXT(1);
// }
