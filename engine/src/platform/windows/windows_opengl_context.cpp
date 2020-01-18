#include "custom_pch.h"
#include "engine/debug/log.h"
#include "engine/platform/opengl_context.h"

#if !defined(CUSTOM_PRECOMPILED_HEADER)
	#include <Windows.h>
	#include <glad/glad.h>
#endif

// https://github.com/glfw/glfw/blob/master/src/wgl_context.c
// https://github.com/spurious/SDL-mirror/blob/master/src/video/windows/SDL_windowsopengl.c
// https://github.com/SFML/SFML/blob/master/src/SFML/Window/Win32/WglContext.cpp

#define OPENGL_LIBRARY_NAME "opengl32.dll"

static void platform_init(HDC hdc);
static void platform_shutdown();

//
// API implementation
//

namespace custom
{
	void Opengl_Context::init(uptr graphics)
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

#include "wgl_tiny.h"

struct Wgl_Context
{
	HINSTANCE instance;

	// OGL functions
	CreateContext_func     * CreateContext;
	DeleteContext_func     * DeleteContext;
	GetProcAddress_func    * GetProcAddress;
	MakeCurrent_func       * MakeCurrent;
	ShareLists_func        * ShareLists;

	// EXT functions
	GetExtensionsStringEXT_func    * GetExtensionsStringEXT;
	SwapIntervalEXT_func           * SwapIntervalEXT;
	GetSwapIntervalEXT_func        * GetSwapIntervalEXT;

	// ARB functions
	GetExtensionsStringARB_func    * GetExtensionsStringARB;
	CreateContextAttribsARB_func   * CreateContextAttribsARB;
	GetPixelFormatAttribivARB_func * GetPixelFormatAttribivARB;
	ChoosePixelFormatARB_func      * ChoosePixelFormatARB;

	// EXT extensions
	bool EXT_framebuffer_sRGB;
	bool EXT_create_context_es2_profile;
	bool EXT_swap_control;
	bool EXT_swap_control_tear;
	// bool EXT_colorspace;

	// ARB extensions
	bool ARB_multisample;
	bool ARB_framebuffer_sRGB;
	bool ARB_create_context_robustness;
	bool ARB_create_context_no_error;
	bool ARB_pixel_format;
	bool ARB_context_flush_control;
	// bool ARB_create_context;
	// bool ARB_create_context_profile;
};
static Wgl_Context wgl;

void * wgl_get_proc_address(cstring name) {
	CUSTOM_ASSERT(name, "null GL procedure name");
	void * address = wgl.GetProcAddress(name);
	if (!address) {
		address = GetProcAddress(wgl.instance, name);
	}
	return address;
}

cstring wgl_get_extensions_string(HDC hdc) {
	if (wgl.GetExtensionsStringARB) {
		return wgl.GetExtensionsStringARB(hdc);
	}
	if (wgl.GetExtensionsStringEXT) {
		return wgl.GetExtensionsStringEXT();
	}
	return NULL;
}

static bool contains_subword(cstring container, cstring value) {
	CUSTOM_ASSERT(value, "extension is nullptr");
	CUSTOM_ASSERT(*value != '\0', "extension is empty");
	CUSTOM_ASSERT(!strchr(value, ' '), "extension contains spaces: '%s'", value);

	cstring start = container;
	while (true)
	{
		cstring where = strstr(start, value);
		if (!where) { return false; }

		// @Note: make sure a word has been found, not a substring
		cstring terminator = where + strlen(value);
		if (where == start || *(where - 1) == ' ') {
			if (*terminator == ' ' || *terminator == '\0')
				break;
		}

		start = terminator;
	}

	return true;
}

#define LOAD_OPENGL_FUNCTION(name, required) {\
	wgl.name = (name##_func *)GetProcAddress(wgl.instance, "wgl" #name);\
	CUSTOM_ASSERT(wgl.name || !required, "failed to load 'wgl" #name "' from " OPENGL_LIBRARY_NAME);\
}
static void load_opengl_functions() {
	LOAD_OPENGL_FUNCTION(CreateContext,  true);
	LOAD_OPENGL_FUNCTION(DeleteContext,  true);
	LOAD_OPENGL_FUNCTION(GetProcAddress, true);
	LOAD_OPENGL_FUNCTION(MakeCurrent,    true);
	LOAD_OPENGL_FUNCTION(ShareLists,     false);
}
#undef LOAD_OPENGL_FUNCTION

#define LOAD_EXTENSION_FUNCTION(name) wgl.name = (name##_func *)wgl.GetProcAddress("wgl" #name)
static void load_extension_functions_through_dummy() {
	// EXT functions
	LOAD_EXTENSION_FUNCTION(GetExtensionsStringEXT);
	LOAD_EXTENSION_FUNCTION(SwapIntervalEXT);
	LOAD_EXTENSION_FUNCTION(GetSwapIntervalEXT);
	// ARB extensions
	LOAD_EXTENSION_FUNCTION(GetExtensionsStringARB);
	LOAD_EXTENSION_FUNCTION(CreateContextAttribsARB);
	LOAD_EXTENSION_FUNCTION(GetPixelFormatAttribivARB);
	LOAD_EXTENSION_FUNCTION(ChoosePixelFormatARB);
}
#undef LOAD_EXTENSION_FUNCTION

#define CHECK_EXTENSION(name) wgl.name = contains_subword(extensions_string, "WGL_" #name)
void check_extension_through_dummy(HDC hdc) {
	cstring extensions_string = wgl_get_extensions_string(hdc);
	if (!extensions_string) {
		CUSTOM_ASSERT(false, "failed to load extensions string");
		return;
	}
	// EXT extensions
	CHECK_EXTENSION(EXT_framebuffer_sRGB);
	CHECK_EXTENSION(EXT_create_context_es2_profile);
	CHECK_EXTENSION(EXT_swap_control);
	CHECK_EXTENSION(EXT_swap_control_tear);
	// CHECK_EXTENSION(EXT_colorspace);
	// ARB extensions
	CHECK_EXTENSION(ARB_multisample);
	CHECK_EXTENSION(ARB_framebuffer_sRGB);
	CHECK_EXTENSION(ARB_create_context_robustness);
	CHECK_EXTENSION(ARB_create_context_no_error);
	CHECK_EXTENSION(ARB_pixel_format);
	CHECK_EXTENSION(ARB_context_flush_control);
	// CHECK_EXTENSION(ARB_create_context);
	// CHECK_EXTENSION(ARB_create_context_profile);
}
#undef CHECK_EXTENSION

static void load_extensions(HDC hdc) {
	PIXELFORMATDESCRIPTOR dummy_pfd = {};
	dummy_pfd.nSize        = sizeof(dummy_pfd);
	dummy_pfd.nVersion     = 1;
	dummy_pfd.dwFlags      = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	dummy_pfd.iPixelType   = PFD_TYPE_RGBA;
	dummy_pfd.cColorBits   = 8 * 3;
	// dummy_pfd.cDepthBits   = 8 * 3;
	// dummy_pfd.cStencilBits = 8 * 1;

	if (!SetPixelFormat(hdc, ChoosePixelFormat(hdc, &dummy_pfd), &dummy_pfd)) {
		CUSTOM_ASSERT(false, "failed to set dummy pixel format");
		return;
	}

	HGLRC dummy_hrc = wgl.CreateContext(hdc);
	if (!dummy_hrc) {
		CUSTOM_ASSERT(false, "failed to create dummy rendering context");
		return;
	}

	if (!wgl.MakeCurrent(hdc, dummy_hrc)) {
		CUSTOM_ASSERT(false, "failed to make dummy rendering context the current one");
	}
	else {
		load_extension_functions_through_dummy();
		check_extension_through_dummy(hdc);
	}

	wgl.MakeCurrent(hdc, NULL);
	wgl.DeleteContext(dummy_hrc);
}

static void platform_init(HDC hdc) {
	HINSTANCE opengl_handle = LoadLibrary(TEXT(OPENGL_LIBRARY_NAME));
	if (!opengl_handle) {
		CUSTOM_ASSERT(false, "can't load " OPENGL_LIBRARY_NAME);
		return;
	}

	wgl.instance = opengl_handle;
	load_opengl_functions();
	load_extensions(hdc);
}

static void platform_shutdown() {
	FreeLibrary(wgl.instance);
}

#define ADD_ATTRIBUTE_KEY(key) {\
	CUSTOM_ASSERT(count < cap, "attributes capacity reached");\
	keys[count++] = key;\
}
int add_atribute_keys(int * keys, int cap) {
	int count = 0;
	ADD_ATTRIBUTE_KEY(WGL_SUPPORT_OPENGL_ARB);
	ADD_ATTRIBUTE_KEY(WGL_DRAW_TO_WINDOW_ARB);
	ADD_ATTRIBUTE_KEY(WGL_PIXEL_TYPE_ARB);
	ADD_ATTRIBUTE_KEY(WGL_ACCELERATION_ARB);
	ADD_ATTRIBUTE_KEY(WGL_RED_BITS_ARB);
	ADD_ATTRIBUTE_KEY(WGL_RED_SHIFT_ARB);
	ADD_ATTRIBUTE_KEY(WGL_GREEN_BITS_ARB);
	ADD_ATTRIBUTE_KEY(WGL_GREEN_SHIFT_ARB);
	ADD_ATTRIBUTE_KEY(WGL_BLUE_BITS_ARB);
	ADD_ATTRIBUTE_KEY(WGL_BLUE_SHIFT_ARB);
	ADD_ATTRIBUTE_KEY(WGL_ALPHA_BITS_ARB);
	ADD_ATTRIBUTE_KEY(WGL_ALPHA_SHIFT_ARB);
	ADD_ATTRIBUTE_KEY(WGL_DEPTH_BITS_ARB);
	ADD_ATTRIBUTE_KEY(WGL_STENCIL_BITS_ARB);
	ADD_ATTRIBUTE_KEY(WGL_ACCUM_BITS_ARB);
	ADD_ATTRIBUTE_KEY(WGL_ACCUM_RED_BITS_ARB);
	ADD_ATTRIBUTE_KEY(WGL_ACCUM_GREEN_BITS_ARB);
	ADD_ATTRIBUTE_KEY(WGL_ACCUM_BLUE_BITS_ARB);
	ADD_ATTRIBUTE_KEY(WGL_ACCUM_ALPHA_BITS_ARB);
	ADD_ATTRIBUTE_KEY(WGL_AUX_BUFFERS_ARB);
	ADD_ATTRIBUTE_KEY(WGL_STEREO_ARB);
	ADD_ATTRIBUTE_KEY(WGL_DOUBLE_BUFFER_ARB);

	if (wgl.ARB_multisample) {
		ADD_ATTRIBUTE_KEY(WGL_SAMPLES_ARB);
	}

	// @Note: is desktop OpenGL?
	if (wgl.ARB_framebuffer_sRGB || wgl.EXT_framebuffer_sRGB) {
		ADD_ATTRIBUTE_KEY(WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB);
	}

	// @Note: is not desktop OpenGL?
	// if (wgl.EXT_colorspace) {
	// 	ADD_ATTRIBUTE_KEY(WGL_COLORSPACE_EXT);
	// }

	return count;
}
#undef ADD_ATTRIBUTE

int choose_pixel_format_arb(HDC hdc) {
	// https://www.khronos.org/registry/OpenGL/extensions/ARB/WGL_ARB_pixel_format.txt
	// The number of pixel formats for the device context.
	// The <iLayerPlane> and <iPixelFormat> parameters are ignored
	int const attribute = WGL_NUMBER_PIXEL_FORMATS_ARB;
	int nativeCount;
	if (!wgl.GetPixelFormatAttribivARB(hdc, NULL, NULL, 1, &attribute, &nativeCount)) {
		CUSTOM_ASSERT(false, "failed to get pixel format attribute");
	}

	int const attr_cap = 40;
	int attr_keys[attr_cap] = {};
	int attr_vals[attr_cap] = {};
	int attr_count = add_atribute_keys(attr_keys, attr_cap);

	return 0;
}
