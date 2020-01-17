#include "custom_pch.h"
#include "windows_opengl_context.h"
#include "engine/debug/log.h"

#if !defined(CUSTOM_PRECOMPILED_HEADER)
	#include <Windows.h>
	#include <glad/glad.h>
#endif

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

#include "wgl_tiny.h"

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

		bool ARB_multisample;
		bool ARB_framebuffer_sRGB;
		bool EXT_framebuffer_sRGB;
		bool ARB_create_context;
		bool ARB_create_context_profile;
		bool EXT_create_context_es2_profile;
		bool ARB_create_context_robustness;
		bool ARB_create_context_no_error;
		bool EXT_swap_control;
		bool EXT_colorspace;
		bool ARB_pixel_format;
		bool ARB_context_flush_control;
	};
}
static custom::Wgl_Context wgl;

namespace custom
{
	void * wgl_get_proc_address(cstring name) {
		CUSTOM_ASSERT(name, "null GL procedure name");
		void * address = wgl.GetProcAddress(name);
		if (!address) {
			address = GetProcAddress(wgl.instance, name);
		}
		return address;
	}

	static bool contains_subword(cstring container, cstring value) {
		cstring start = container;
		while (true)
		{
			cstring where = strstr(start, value);
			if (!where) { return false; }

			cstring terminator = where + strlen(value);
			if (where == start || *(where - 1) == ' ') {
				if (*terminator == ' ' || *terminator == '\0')
					break;
			}

			start = terminator;
		}

		return true;
	}

	bool wgl_contains_extension(HDC hdc, cstring value) {
		cstring container = NULL;

		if (wgl.GetExtensionsStringARB)
			container = wgl.GetExtensionsStringARB(hdc);
		else if (wgl.GetExtensionsStringEXT)
			container = wgl.GetExtensionsStringEXT();

		if (!container) { return false; }
		return contains_subword(container, value);
	}
}

#define LOAD_OPENGL_FUNCTION(name) wgl.name = (name##_func *)GetProcAddress(wgl.instance, "wgl" #name)
static void load_opengl_functions() {
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
static void load_extension_functions_through_dummy() {
	LOAD_EXTENSION_FUNCTION(GetExtensionsStringEXT);
	LOAD_EXTENSION_FUNCTION(SwapIntervalEXT);
	LOAD_EXTENSION_FUNCTION(GetExtensionsStringARB);
	LOAD_EXTENSION_FUNCTION(CreateContextAttribsARB);
	LOAD_EXTENSION_FUNCTION(GetPixelFormatAttribivARB);
}
#undef LOAD_EXTENSION_FUNCTION

#define CHECK_EXTENSION(name) wgl.name = custom::wgl_contains_extension(hdc, "WGL_" #name)
void check_extension_through_dummy(HDC hdc) {
	CHECK_EXTENSION(ARB_multisample);
	CHECK_EXTENSION(ARB_framebuffer_sRGB);
	CHECK_EXTENSION(EXT_framebuffer_sRGB);
	CHECK_EXTENSION(ARB_create_context);
	CHECK_EXTENSION(ARB_create_context_profile);
	CHECK_EXTENSION(EXT_create_context_es2_profile);
	CHECK_EXTENSION(ARB_create_context_robustness);
	CHECK_EXTENSION(ARB_create_context_no_error);
	CHECK_EXTENSION(EXT_swap_control);
	CHECK_EXTENSION(EXT_colorspace);
	CHECK_EXTENSION(ARB_pixel_format);
	CHECK_EXTENSION(ARB_context_flush_control);
}
#undef CHECK_EXTENSION

static void load_extensions(HDC hdc) {
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
		load_extension_functions_through_dummy();
		check_extension_through_dummy(hdc);
	}

	wgl.MakeCurrent(current_hdc, current_hrc);
	wgl.DeleteContext(dummy_hrc);
}

static void platform_init(HDC hdc) {
	HINSTANCE opengl_handle = LoadLibrary("opengl32.dll");
	if (!opengl_handle) {
		CUSTOM_ASSERT(false, "can't load opengl32.dll");
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
