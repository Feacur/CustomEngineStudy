#include "custom_pch.h"
#include "engine/debug/log.h"
#include "engine/platform/opengl_context.h"

#if !defined(CUSTOM_PRECOMPILED_HEADER)
	#include <Windows.h>
	#include <glad/glad.h>
#endif

// https://www.khronos.org/registry/OpenGL/extensions/ARB/WGL_ARB_pixel_format.txt
// https://docs.microsoft.com/en-us/windows/win32/api/wingdi/ns-wingdi-pixelformatdescriptor

// https://github.com/glfw/glfw/blob/master/src/wgl_context.c
// https://github.com/spurious/SDL-mirror/blob/master/src/video/windows/SDL_windowsopengl.c
// https://github.com/SFML/SFML/blob/master/src/SFML/Window/Win32/WglContext.cpp

#define OPENGL_LIBRARY_NAME "opengl32.dll"

static void platform_init(HDC hdc);
static void platform_shutdown();

constexpr inline bool bits_are_set(DWORD container, DWORD bits) {
	return (container & bits) == bits;
}

#define ALLOCATE(type, count) (type *)malloc(count * sizeof(type))
#define CREATE_ARRAY(type, count, name) type * name = ALLOCATE(type, count)

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

struct Pixel_Format
{
	int redBits;
	int greenBits;
	int blueBits;
	int alphaBits;
	int depthBits;
	int stencilBits;
};

struct Pixel_Format_Aux
{
	int  redShift;
	int  greenShift;
	int  blueShift;
	int  alphaShift;
	//
	int  accumRedShift;
	int  accumGreenShift;
	int  accumBlueShift;
	int  accumAlphaShift;
	//
	int  auxBuffers;
	int  samples;
	//
	bool stereo;
	bool sRGB;
	bool doublebuffer;
	// bool transparent;
};

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
	// ChoosePixelFormatARB_func      * ChoosePixelFormatARB;

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

static cstring wgl_get_extensions_string(HDC hdc) {
	if (wgl.GetExtensionsStringARB) {
		return wgl.GetExtensionsStringARB(hdc);
	}
	if (wgl.GetExtensionsStringEXT) {
		return wgl.GetExtensionsStringEXT();
	}
	return NULL;
}

static bool contains_full_word(cstring container, cstring value) {
	CUSTOM_ASSERT(value, "value is nullptr");
	CUSTOM_ASSERT(*value != '\0', "value is empty");
	CUSTOM_ASSERT(!strchr(value, ' '), "value contains spaces: '%s'", value);

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
	// LOAD_EXTENSION_FUNCTION(ChoosePixelFormatARB);
}
#undef LOAD_EXTENSION_FUNCTION

#define CHECK_EXTENSION(name) wgl.name = contains_full_word(extensions_string, "WGL_" #name)
static void check_extension_through_dummy(HDC hdc) {
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

#define ADD_ATTRIBUTE_KEY(key) {\
	CUSTOM_ASSERT(count < cap, "attributes capacity reached");\
	keys[count++] = key;\
}
static int add_atribute_keys(int * keys, int cap) {
	int count = 0;
	ADD_ATTRIBUTE_KEY(WGL_SUPPORT_OPENGL_ARB);
	ADD_ATTRIBUTE_KEY(WGL_DRAW_TO_WINDOW_ARB);
	ADD_ATTRIBUTE_KEY(WGL_PIXEL_TYPE_ARB);
	ADD_ATTRIBUTE_KEY(WGL_ACCELERATION_ARB);
	ADD_ATTRIBUTE_KEY(WGL_RED_BITS_ARB);
	// ADD_ATTRIBUTE_KEY(WGL_RED_SHIFT_ARB);
	ADD_ATTRIBUTE_KEY(WGL_GREEN_BITS_ARB);
	// ADD_ATTRIBUTE_KEY(WGL_GREEN_SHIFT_ARB);
	ADD_ATTRIBUTE_KEY(WGL_BLUE_BITS_ARB);
	// ADD_ATTRIBUTE_KEY(WGL_BLUE_SHIFT_ARB);
	ADD_ATTRIBUTE_KEY(WGL_ALPHA_BITS_ARB);
	// ADD_ATTRIBUTE_KEY(WGL_ALPHA_SHIFT_ARB);
	ADD_ATTRIBUTE_KEY(WGL_DEPTH_BITS_ARB);
	ADD_ATTRIBUTE_KEY(WGL_STENCIL_BITS_ARB);
	// ADD_ATTRIBUTE_KEY(WGL_ACCUM_BITS_ARB);
	// ADD_ATTRIBUTE_KEY(WGL_ACCUM_RED_BITS_ARB);
	// ADD_ATTRIBUTE_KEY(WGL_ACCUM_GREEN_BITS_ARB);
	// ADD_ATTRIBUTE_KEY(WGL_ACCUM_BLUE_BITS_ARB);
	// ADD_ATTRIBUTE_KEY(WGL_ACCUM_ALPHA_BITS_ARB);
	// ADD_ATTRIBUTE_KEY(WGL_AUX_BUFFERS_ARB);
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

int get_value(int * keys, int * vals, int key) {
	return 0;
}

#define GET_ATTRIBUTE_VALUE(key) get_value(attr_keys, attr_vals, key)
static int choose_pixel_format_arb(HDC hdc) {
	// The number of pixel formats for the device context.
	// The <iLayerPlane> and <iPixelFormat> parameters are ignored
	int const formats_request = WGL_NUMBER_PIXEL_FORMATS_ARB;
	int formats_count;
	if (!wgl.GetPixelFormatAttribivARB(hdc, 0, 0, 1, &formats_request, &formats_count)) {
		CUSTOM_ASSERT(false, "failed to count pixel formats");
		return 0;
	}

	int const attr_cap = 40;
	int attr_keys[attr_cap] = {};
	int attr_vals[attr_cap] = {};
	int attr_count = add_atribute_keys(attr_keys, attr_cap);

	int pf_count = 0;
	CREATE_ARRAY(Pixel_Format, formats_count, pixel_formats);
	for (int i = 0; i < formats_count; ++i)
	{
		int pixel_format_id = i + 1;
		if (!wgl.GetPixelFormatAttribivARB(hdc, pixel_format_id, 0, attr_count, attr_keys, attr_vals)) {
			CUSTOM_WARN("failed to get pixel format %d values", pixel_format_id);
			continue;
		}

		Pixel_Format pf = {};
		pf.alphaBits = GET_ATTRIBUTE_VALUE(WGL_ALPHA_BITS_ARB);

		pixel_formats[pf_count++] = pf;
	}

	// @Todo: search for the best match

	free(pixel_formats);

	return 0;
}

static int choose_pixel_format_legacy(HDC hdc) {
	int formats_count = DescribePixelFormat(
		hdc, 1, sizeof(PIXELFORMATDESCRIPTOR), NULL
	);

	int pf_count = 0;
	CREATE_ARRAY(Pixel_Format, formats_count, pixel_formats);
	for (int i = 0; i < formats_count; ++i)
	{
		int pixel_format_id = i + 1;
		PIXELFORMATDESCRIPTOR pfd;
		if (!DescribePixelFormat(hdc, pixel_format_id, sizeof(pfd), &pfd))
		{
			CUSTOM_WARN("failed to describe pixel format %d", pixel_format_id);
			continue;
		}
		// should support
		if (!bits_are_set(pfd.dwFlags, PFD_DRAW_TO_WINDOW)) { continue; }
		if (!bits_are_set(pfd.dwFlags, PFD_SUPPORT_OPENGL)) { continue; }
		// if (!bits_are_set(pfd.dwFlags, PFD_GENERIC_ACCELERATED)) { continue; }
		
		// should not support
		// if (bits_are_set(pfd.dwFlags, PFD_DRAW_TO_BITMAP)) { continue; }
		if (bits_are_set(pfd.dwFlags, PFD_SUPPORT_GDI)) { continue; }
		// if (!bits_are_set(pfd.dwFlags, PFD_GENERIC_FORMAT)) { continue; }

		Pixel_Format pf = {};
		pf.redBits     = pfd.cRedBits;
		pf.greenBits   = pfd.cGreenBits;
		pf.blueBits    = pfd.cBlueBits;
		pf.alphaBits   = pfd.cAlphaBits;
		pf.depthBits   = pfd.cDepthBits;
		pf.stencilBits = pfd.cStencilBits;

		Pixel_Format_Aux pfa = {};
		pfa.redShift = pfd.cRedShift;
		pfa.greenShift = pfd.cGreenShift;
		pfa.blueShift = pfd.cBlueShift;
		pfa.alphaShift = pfd.cAlphaShift;
		//
		pfa.accumRedShift = pfd.cAccumRedBits;
		pfa.accumGreenShift = pfd.cAccumGreenBits;
		pfa.accumBlueShift = pfd.cAccumBlueBits;
		pfa.accumAlphaShift = pfd.cAccumAlphaBits;
		//
		pfa.auxBuffers = pfd.cAuxBuffers;
		pfa.samples = 0;
		//
		pfa.stereo = bits_are_set(pfd.dwFlags, PFD_STEREO);
		pfa.sRGB = pfd.iPixelType == PFD_TYPE_RGBA;
		pfa.doublebuffer = bits_are_set(pfd.dwFlags, PFD_DOUBLEBUFFER);
		// pfa.transparent = true;

		pixel_formats[pf_count++] = pf;
	}

	// @Todo: search for the best match

	free(pixel_formats);

	return 0;
}

static void craete_context(HDC hdc) {
	int pixel_format;
	if (wgl.ARB_pixel_format) {
		pixel_format = choose_pixel_format_arb(hdc);
	}
	else {
		pixel_format = choose_pixel_format_legacy(hdc);
	}

	PIXELFORMATDESCRIPTOR pfd;
	if (!DescribePixelFormat(hdc, pixel_format, sizeof(pfd), &pfd)) {
		CUSTOM_ASSERT(false, "failed to describe pixel format %d", pixel_format);
		return;
	}

	if (!SetPixelFormat(hdc, pixel_format, &pfd)) {
		CUSTOM_ASSERT(false, "failed to set pixel format %d", pixel_format);
		return;
	}

	int const attr_cap = 40;
	int attr_pair[attr_cap * 2] = {};

	HGLRC share_hrc = NULL;
	HGLRC hrc = wgl.CreateContextAttribsARB(hdc, share_hrc, attr_pair);
	if (!hrc) {
		DWORD const error = GetLastError();
		if (bits_are_set(error, ERROR_INVALID_VERSION_ARB)) {
			CUSTOM_ASSERT(false, "'0x%x' failed to create context: invalid version", error);
		}
		else if (bits_are_set(error, ERROR_INVALID_PROFILE_ARB)) {
			CUSTOM_ASSERT(false, "'0x%x' failed to create context: invalid profile", error);
		}
		else if (bits_are_set(error, ERROR_INCOMPATIBLE_DEVICE_CONTEXTS_ARB)) {
			CUSTOM_ASSERT(false, "'0x%x' failed to create context: incopatible device context", error);
		}
		else {
			CUSTOM_ASSERT(false, "'0x%x' failed to create context: unknown", error);
		}
		return;
	}

	// ???
	HGLRC hrc_2 = wgl.CreateContext(hdc);

	if (share_hrc) {
		wgl.ShareLists(share_hrc, hrc);
	}
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

	craete_context(hdc);

	int glad_status = gladLoadGLLoader((GLADloadproc)wgl_get_proc_address);
	CUSTOM_ASSERT(glad_status, "failed to initialize glad");
}

static void platform_shutdown() {
	FreeLibrary(wgl.instance);
}
