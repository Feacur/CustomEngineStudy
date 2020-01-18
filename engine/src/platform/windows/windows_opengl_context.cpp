#include "custom_pch.h"
#include "engine/debug/log.h"
#include "platform/opengl_context.h"

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

struct Pixel_Format
{
	uptr id;
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
	int  accumRedBits;
	int  accumGreenBits;
	int  accumBlueBits;
	int  accumAlphaBits;
	//
	int  auxBuffers;
	int  samples;
	//
	bool stereo;
	bool doublebuffer;
	// bool sRGB;
	// bool transparent;
};

struct Context_Settings
{
	int  profile_api;
	int  forward;
	int  profile_bit;
	//
	int  major_version;
	int  minor_version;
	int  robustness;
	int  release_behaviour;
	bool debug;
	bool opengl_no_error;
};

static void * wgl_get_proc_address(cstring name);
static void platform_init_wgl(HDC dummy_hdc);
static void platform_create_context(HDC hdc, HGLRC share_hrc, Context_Settings settings, Pixel_Format pf_hint);
static void platform_shutdown();
static void platform_swap_interval(uptr display, HDC hdc, s32);
static void platform_swap_buffers(uptr display, HDC hdc);

static constexpr inline bool bits_are_set(DWORD container, DWORD bits) {
	return (container & bits) == bits;
}

#if !defined(CUSTOM_SHIPPING)
static void log_last_error() {
	DWORD const error = GetLastError();
	if (!error) { return; }

	LPTSTR message_buffer = NULL;
	DWORD size = FormatMessage(
		FORMAT_MESSAGE_FROM_SYSTEM
		| FORMAT_MESSAGE_ALLOCATE_BUFFER
		| FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, error,
		MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT),
		(LPTSTR)&message_buffer, 0,
		NULL
	);

	if (size) {
		CUSTOM_ERROR("'0x%x' system error: %s", error, message_buffer);
		LocalFree(message_buffer);
	}
	else {
		CUSTOM_ERROR("'0x%x' system error: unknown", error);
	}
}
	#define LOG_LAST_ERROR() log_last_error()
#else
	#define LOG_LAST_ERROR()
#endif

#define ALLOCATE(type, count) (type *)malloc(count * sizeof(type))
#define CREATE_ARRAY(type, count, name) type * name = ALLOCATE(type, count)

//
// API implementation
//

namespace custom
{
	void Opengl_Context::init(uptr graphics, uptr dummy_graphics)
	{
		Context_Settings settings = {};
		settings.profile_api       = 0; // 0 as OpenGL?, 1 as OpenGL ES?
		settings.forward           = false;
		settings.profile_bit       = 0; // WGL_CONTEXT_CORE_PROFILE_BIT_ARB, WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB
		settings.major_version     = 4;
		settings.minor_version     = 3;
		settings.robustness        = 0; // WGL_NO_RESET_NOTIFICATION_ARB, WGL_LOSE_CONTEXT_ON_RESET_ARB
		settings.release_behaviour = 0; // WGL_CONTEXT_RELEASE_BEHAVIOR_NONE_ARB, WGL_CONTEXT_RELEASE_BEHAVIOR_FLUSH_ARB
		settings.debug             = false;
		settings.opengl_no_error   = false;

		Pixel_Format pf_hint = {};
		pf_hint.redBits     =  8;
		pf_hint.greenBits   =  8;
		pf_hint.blueBits    =  8;
		pf_hint.alphaBits   =  8;
		pf_hint.depthBits   = 24;
		pf_hint.stencilBits =  8;

		platform_init_wgl((HDC)dummy_graphics);

		platform_create_context((HDC)graphics, NULL, settings, pf_hint);

		int glad_status = gladLoadGLLoader((GLADloadproc)wgl_get_proc_address);
		LOG_LAST_ERROR();
		CUSTOM_ASSERT(glad_status, "failed to initialize glad");
	}

	void Opengl_Context::shutdown()
	{
		platform_shutdown();
	}
	
	void Opengl_Context::swap_interval(uptr display, uptr graphics, s32 value)
	{
		platform_swap_interval(display, (HDC)graphics, value);
	}

	void Opengl_Context::swap_buffers(uptr display, uptr graphics)
	{
		platform_swap_buffers(display, (HDC)graphics);
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
	GetCurrentDC_func      * GetCurrentDC;
	GetCurrentContext_func * GetCurrentContext;

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
	bool EXT_colorspace;

	// ARB extensions
	bool ARB_multisample;
	bool ARB_framebuffer_sRGB;
	bool ARB_create_context_robustness;
	bool ARB_create_context_no_error;
	bool ARB_pixel_format;
	bool ARB_context_flush_control;
	bool ARB_create_context;
	bool ARB_create_context_profile;
};
static Wgl_Context wgl;

static void * wgl_get_proc_address(cstring name) {
	if (!name) { return NULL; }
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
	LOAD_OPENGL_FUNCTION(ShareLists,     true);
	LOAD_OPENGL_FUNCTION(GetCurrentDC,   true);
	LOAD_OPENGL_FUNCTION(GetCurrentContext, true);
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
static void check_extension_through_dummy(HDC dummy_hdc) {
	cstring extensions_string = wgl_get_extensions_string(dummy_hdc);
	if (!extensions_string) {
		CUSTOM_ASSERT(false, "failed to load extensions string");
		return;
	}
	// EXT extensions
	CHECK_EXTENSION(EXT_framebuffer_sRGB);
	CHECK_EXTENSION(EXT_create_context_es2_profile);
	CHECK_EXTENSION(EXT_swap_control);
	CHECK_EXTENSION(EXT_swap_control_tear);
	CHECK_EXTENSION(EXT_colorspace);
	// ARB extensions
	CHECK_EXTENSION(ARB_multisample);
	CHECK_EXTENSION(ARB_framebuffer_sRGB);
	CHECK_EXTENSION(ARB_create_context_robustness);
	CHECK_EXTENSION(ARB_create_context_no_error);
	CHECK_EXTENSION(ARB_pixel_format);
	CHECK_EXTENSION(ARB_context_flush_control);
	CHECK_EXTENSION(ARB_create_context);
	CHECK_EXTENSION(ARB_create_context_profile);
}
#undef CHECK_EXTENSION

static void load_extensions(HDC dummy_hdc) {
	PIXELFORMATDESCRIPTOR dummy_pfd = {};
	dummy_pfd.nSize        = sizeof(dummy_pfd);
	dummy_pfd.nVersion     = 1;
	dummy_pfd.dwFlags      = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	dummy_pfd.iPixelType   = PFD_TYPE_RGBA;
	dummy_pfd.cColorBits   = 8 * 3;
	dummy_pfd.cDepthBits   = 8 * 3;
	dummy_pfd.cStencilBits = 8 * 1;

	int dummy_pixel_format_id = ChoosePixelFormat(dummy_hdc, &dummy_pfd);
	if (!dummy_pixel_format_id) {
		LOG_LAST_ERROR();
		CUSTOM_ASSERT(false, "failed to describe dummy pixel format");
		return;
	}

	if (!SetPixelFormat(dummy_hdc, dummy_pixel_format_id, &dummy_pfd)) {
		LOG_LAST_ERROR();
		CUSTOM_ASSERT(false, "failed to set dummy pixel format");
		return;
	}

	HGLRC dummy_hrc = wgl.CreateContext(dummy_hdc);
	if (!dummy_hrc) {
		CUSTOM_ASSERT(false, "failed to create dummy rendering context");
		return;
	}

	HDC current_hdc = wgl.GetCurrentDC();
	HGLRC current_hrc = wgl.GetCurrentContext();

	if (!wgl.MakeCurrent(dummy_hdc, dummy_hrc)) {
		CUSTOM_ASSERT(false, "failed to make dummy rendering context the current one");
	}
	else {
		load_extension_functions_through_dummy();
		check_extension_through_dummy(dummy_hdc);
	}

	wgl.MakeCurrent(current_hdc, current_hrc);
	wgl.DeleteContext(dummy_hrc);
}

#define ADD_ATTRIBUTE_KEY(key) {\
	CUSTOM_ASSERT(count < cap, "attributes capacity reached");\
	keys[count++] = key;\
}
static int add_atribute_keys(int * keys, int cap, Context_Settings settings) {
	int count = 0;
	ADD_ATTRIBUTE_KEY(WGL_SUPPORT_OPENGL_ARB);
	ADD_ATTRIBUTE_KEY(WGL_DRAW_TO_WINDOW_ARB);
	ADD_ATTRIBUTE_KEY(WGL_PIXEL_TYPE_ARB);
	ADD_ATTRIBUTE_KEY(WGL_ACCELERATION_ARB);
	ADD_ATTRIBUTE_KEY(WGL_DOUBLE_BUFFER_ARB);
	//
	ADD_ATTRIBUTE_KEY(WGL_RED_BITS_ARB);
	ADD_ATTRIBUTE_KEY(WGL_GREEN_BITS_ARB);
	ADD_ATTRIBUTE_KEY(WGL_BLUE_BITS_ARB);
	ADD_ATTRIBUTE_KEY(WGL_ALPHA_BITS_ARB);
	ADD_ATTRIBUTE_KEY(WGL_DEPTH_BITS_ARB);
	ADD_ATTRIBUTE_KEY(WGL_STENCIL_BITS_ARB);
	//
	// ADD_ATTRIBUTE_KEY(WGL_RED_SHIFT_ARB);
	// ADD_ATTRIBUTE_KEY(WGL_GREEN_SHIFT_ARB);
	// ADD_ATTRIBUTE_KEY(WGL_BLUE_SHIFT_ARB);
	// ADD_ATTRIBUTE_KEY(WGL_ALPHA_SHIFT_ARB);
	//
	// ADD_ATTRIBUTE_KEY(WGL_ACCUM_BITS_ARB);
	// ADD_ATTRIBUTE_KEY(WGL_ACCUM_RED_BITS_ARB);
	// ADD_ATTRIBUTE_KEY(WGL_ACCUM_GREEN_BITS_ARB);
	// ADD_ATTRIBUTE_KEY(WGL_ACCUM_BLUE_BITS_ARB);
	// ADD_ATTRIBUTE_KEY(WGL_ACCUM_ALPHA_BITS_ARB);
	//
	// ADD_ATTRIBUTE_KEY(WGL_AUX_BUFFERS_ARB);
	// ADD_ATTRIBUTE_KEY(WGL_TRANSPARENT_ARB);

	if (wgl.ARB_multisample) {
		ADD_ATTRIBUTE_KEY(WGL_SAMPLES_ARB);
	}

	if (settings.profile_api == 0) {
		if (wgl.ARB_framebuffer_sRGB || wgl.EXT_framebuffer_sRGB) {
			ADD_ATTRIBUTE_KEY(WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB);
		}
	}
	else if (settings.profile_api == 1) {
		if (wgl.EXT_colorspace) {
			ADD_ATTRIBUTE_KEY(WGL_COLORSPACE_EXT);
		}
	}

	return count;
}
#undef ADD_ATTRIBUTE

int get_value(int * keys, int * vals, int key) {
	for (int i = 0; keys[i]; ++i) {
		if (keys[i] == key) { return vals[i]; }
	}
	return 0;
}

#define GET_ATTRIBUTE_VALUE(key) get_value(attr_keys, attr_vals, key)
static Pixel_Format * allocate_pixel_formats_arb(HDC hdc, Context_Settings settings) {
	// The number of pixel formats for the device context.
	// The <iLayerPlane> and <iPixelFormat> parameters are ignored
	int const formats_request = WGL_NUMBER_PIXEL_FORMATS_ARB;
	int formats_count;
	if (!wgl.GetPixelFormatAttribivARB(hdc, 0, 0, 1, &formats_request, &formats_count)) {
		CUSTOM_ASSERT(false, "failed to count pixel formats");
		return NULL;
	}
	if (!formats_count) { return NULL; }

	int const attr_cap = 64;
	int attr_keys[attr_cap] = {};
	int attr_vals[attr_cap] = {};
	int attr_count = add_atribute_keys(attr_keys, attr_cap, settings);

	int pf_count = 0;
	CREATE_ARRAY(Pixel_Format, formats_count, pixel_formats);
	for (int i = 0; i < formats_count; ++i) {
		int pixel_format_id = i + 1;
		if (!wgl.GetPixelFormatAttribivARB(hdc, pixel_format_id, 0, attr_count, attr_keys, attr_vals)) {
			CUSTOM_WARN("failed to get pixel format %d values", pixel_format_id);
			continue;
		}
		// should support
		if (!GET_ATTRIBUTE_VALUE(WGL_DRAW_TO_WINDOW_ARB)) { continue; }
		if (!GET_ATTRIBUTE_VALUE(WGL_SUPPORT_OPENGL_ARB)) { continue; }
		if (GET_ATTRIBUTE_VALUE(WGL_PIXEL_TYPE_ARB) != WGL_TYPE_RGBA_ARB) {
			continue;
		}
		if (settings.profile_api == 0) {
			if (wgl.ARB_framebuffer_sRGB || wgl.EXT_framebuffer_sRGB) {
				if (!GET_ATTRIBUTE_VALUE(WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB)) {
					continue;
				}
			}
		}
		else if (settings.profile_api == 1) {
			if (wgl.EXT_colorspace) {
				if (GET_ATTRIBUTE_VALUE(WGL_COLORSPACE_EXT) != WGL_COLORSPACE_SRGB_EXT) {
					continue;
				}
			}
		}

		// should not support
		if (GET_ATTRIBUTE_VALUE(PFD_SUPPORT_GDI)) { continue; }
		if (GET_ATTRIBUTE_VALUE(PFD_GENERIC_FORMAT)) { continue; }
		if (GET_ATTRIBUTE_VALUE(WGL_ACCELERATION_ARB) == WGL_NO_ACCELERATION_ARB) {
			continue;
		}

		Pixel_Format pf = {};
		pf.id = pixel_format_id;
		pf.redBits     = GET_ATTRIBUTE_VALUE(WGL_RED_BITS_ARB);
		pf.greenBits   = GET_ATTRIBUTE_VALUE(WGL_GREEN_BITS_ARB);
		pf.blueBits    = GET_ATTRIBUTE_VALUE(WGL_BLUE_BITS_ARB);
		pf.alphaBits   = GET_ATTRIBUTE_VALUE(WGL_ALPHA_BITS_ARB);
		pf.depthBits   = GET_ATTRIBUTE_VALUE(WGL_DEPTH_BITS_ARB);
		pf.stencilBits = GET_ATTRIBUTE_VALUE(WGL_STENCIL_BITS_ARB);

		pixel_formats[pf_count++] = pf;

		/*
		Pixel_Format_Aux pfa = {};
		pfa.redShift   = GET_ATTRIBUTE_VALUE(WGL_RED_SHIFT_ARB);
		pfa.greenShift = GET_ATTRIBUTE_VALUE(WGL_GREEN_SHIFT_ARB);
		pfa.blueShift  = GET_ATTRIBUTE_VALUE(WGL_BLUE_SHIFT_ARB);
		pfa.alphaShift = GET_ATTRIBUTE_VALUE(WGL_ALPHA_SHIFT_ARB);
		//
		pfa.accumRedBits   = GET_ATTRIBUTE_VALUE(WGL_ACCUM_RED_BITS_ARB);
		pfa.accumGreenBits = GET_ATTRIBUTE_VALUE(WGL_ACCUM_GREEN_BITS_ARB);
		pfa.accumBlueBits  = GET_ATTRIBUTE_VALUE(WGL_ACCUM_BLUE_BITS_ARB);
		pfa.accumAlphaBits = GET_ATTRIBUTE_VALUE(WGL_ACCUM_ALPHA_BITS_ARB);
		//
		pfa.auxBuffers = GET_ATTRIBUTE_VALUE(WGL_AUX_BUFFERS_ARB);
		pfa.samples = GET_ATTRIBUTE_VALUE(WGL_SAMPLES_ARB);
		//
		pfa.stereo = GET_ATTRIBUTE_VALUE(WGL_STEREO_ARB);
		pfa.doublebuffer = GET_ATTRIBUTE_VALUE(WGL_DOUBLE_BUFFER_ARB);
		// pfa.transparent = GET_ATTRIBUTE_VALUE(WGL_TRANSPARENT_ARB);
		*/
	}

	pixel_formats[pf_count].id = NULL;
	return pixel_formats;
}
#undef GET_ATTRIBUTE_VALUE

static Pixel_Format * allocate_pixel_formats_legacy(HDC hdc) {
	int formats_count = DescribePixelFormat(
		hdc, 1, sizeof(PIXELFORMATDESCRIPTOR), NULL
	);
	LOG_LAST_ERROR();
	if (!formats_count) { return NULL; }

	int pf_count = 0;
	CREATE_ARRAY(Pixel_Format, formats_count, pixel_formats);
	for (int i = 0; i < formats_count; ++i) {
		int pixel_format_id = i + 1;
		PIXELFORMATDESCRIPTOR pfd;
		if (!DescribePixelFormat(hdc, pixel_format_id, sizeof(pfd), &pfd))
		{
			LOG_LAST_ERROR();
			CUSTOM_WARN("failed to describe pixel format %d", pixel_format_id);
			continue;
		}
		// should support
		if (!bits_are_set(pfd.dwFlags, PFD_DRAW_TO_WINDOW)) { continue; }
		if (!bits_are_set(pfd.dwFlags, PFD_SUPPORT_OPENGL)) { continue; }
		if (!bits_are_set(pfd.dwFlags, PFD_GENERIC_ACCELERATED)) { continue; }
		if (pfd.iPixelType != PFD_TYPE_RGBA) { continue; }
		
		// should not support
		if (bits_are_set(pfd.dwFlags, PFD_SUPPORT_GDI)) { continue; }
		if (bits_are_set(pfd.dwFlags, PFD_GENERIC_FORMAT)) { continue; }

		Pixel_Format pf = {};
		pf.id = pixel_format_id;
		pf.redBits     = pfd.cRedBits;
		pf.greenBits   = pfd.cGreenBits;
		pf.blueBits    = pfd.cBlueBits;
		pf.alphaBits   = pfd.cAlphaBits;
		pf.depthBits   = pfd.cDepthBits;
		pf.stencilBits = pfd.cStencilBits;

		pixel_formats[pf_count++] = pf;

		/*
		Pixel_Format_Aux pfa = {};
		pfa.redShift   = pfd.cRedShift;
		pfa.greenShift = pfd.cGreenShift;
		pfa.blueShift  = pfd.cBlueShift;
		pfa.alphaShift = pfd.cAlphaShift;
		//
		pfa.accumRedBits   = pfd.cAccumRedBits;
		pfa.accumGreenBits = pfd.cAccumGreenBits;
		pfa.accumBlueBits  = pfd.cAccumBlueBits;
		pfa.accumAlphaBits = pfd.cAccumAlphaBits;
		//
		pfa.auxBuffers = pfd.cAuxBuffers;
		pfa.samples = 0;
		//
		pfa.stereo = bits_are_set(pfd.dwFlags, PFD_STEREO);
		// pfa.sRGB = pfd.iPixelType == PFD_TYPE_RGBA;
		pfa.doublebuffer = bits_are_set(pfd.dwFlags, PFD_DOUBLEBUFFER);
		// pfa.transparent = true;
		*/
	}

	pixel_formats[pf_count].id = NULL;
	return pixel_formats;
}

static int find_best_pixel_format(Pixel_Format * formats, Pixel_Format pf_hint) {
	Pixel_Format best_match = {};
	for (Pixel_Format * format = formats; format && format->id; ++format)
	{
		if (format->redBits     < pf_hint.redBits)     { continue; }
		if (format->greenBits   < pf_hint.greenBits)   { continue; }
		if (format->blueBits    < pf_hint.blueBits)    { continue; }
		if (format->alphaBits   < pf_hint.alphaBits)   { continue; }
		if (format->depthBits   < pf_hint.depthBits)   { continue; }
		if (format->stencilBits < pf_hint.stencilBits) { continue; }
		best_match = *format;
		break;
	}
	return (int)best_match.id;
}

static int get_pixel_format(HDC hdc, Pixel_Format pf_hint, Context_Settings settings) {
	Pixel_Format * pixel_formats;
	if (wgl.ARB_pixel_format) {
		pixel_formats = allocate_pixel_formats_arb(hdc, settings);
	}
	else {
		pixel_formats = allocate_pixel_formats_legacy(hdc);
	}

	if (!pixel_formats) { return 0; }
	int pixel_format_id = find_best_pixel_format(pixel_formats, pf_hint);
	free(pixel_formats);

	return pixel_format_id;
}

#define SET_ATTRIBUTE(key, value) {\
	CUSTOM_ASSERT(attr_count < attr_cap, "attributes capacity reached");\
	attr_pair[attr_count++] = key;\
	attr_pair[attr_count++] = value;\
}
static HGLRC create_context_arb(HDC hdc, HGLRC share_hrc, Context_Settings settings) {
	int const attr_cap = 64 * 2;
	int attr_pair[attr_cap] = {};
	int attr_count = 0;

	int profile_mask = 0;
	int context_flags = 0;

	if (settings.profile_api == 0) {
		if (settings.forward) {
			context_flags |= WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB;
		}
		if (settings.profile_bit) {
			profile_mask |= settings.profile_bit;
		}
	}
	else if (settings.profile_api == 1) {
		profile_mask |= WGL_CONTEXT_ES2_PROFILE_BIT_EXT;
	}

	if (settings.major_version != 1 || settings.minor_version != 0) {
		SET_ATTRIBUTE(WGL_CONTEXT_MAJOR_VERSION_ARB, settings.major_version);
		SET_ATTRIBUTE(WGL_CONTEXT_MINOR_VERSION_ARB, settings.minor_version);
	}

	if (settings.robustness && wgl.ARB_create_context_robustness) {
		SET_ATTRIBUTE(
			WGL_CONTEXT_RESET_NOTIFICATION_STRATEGY_ARB,
			settings.robustness
		);
		context_flags |= WGL_CONTEXT_ROBUST_ACCESS_BIT_ARB;
	}

	if (settings.release_behaviour && wgl.ARB_context_flush_control) {
		SET_ATTRIBUTE(
			WGL_CONTEXT_RELEASE_BEHAVIOR_ARB,
			settings.release_behaviour
		);
	}

	if (settings.debug) {
		context_flags |= WGL_CONTEXT_DEBUG_BIT_ARB;
	}

	if (settings.opengl_no_error && wgl.ARB_create_context_no_error) {
		SET_ATTRIBUTE(WGL_CONTEXT_OPENGL_NO_ERROR_ARB, true);
	}

	if (context_flags) {
		SET_ATTRIBUTE(WGL_CONTEXT_FLAGS_ARB, context_flags);
	}

	if (profile_mask) {
		SET_ATTRIBUTE(WGL_CONTEXT_PROFILE_MASK_ARB, profile_mask);
	}

	SET_ATTRIBUTE(0, 0);

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
		return NULL;
	}

	return hrc;
}
#undef SET_ATTRIBUTE

static HGLRC create_context_legacy(HDC hdc, HGLRC share_hrc) {
	HGLRC hrc = wgl.CreateContext(hdc);
	if (!hrc) {
		CUSTOM_ASSERT(false, "failed to create context");
		return NULL;
	}

	if (share_hrc && !wgl.ShareLists(share_hrc, hrc)) {
		CUSTOM_ASSERT(false, "failed to share context");
	}

	return hrc;
}

static void platform_init_wgl(HDC dummy_hdc) {
	HINSTANCE opengl_handle = LoadLibrary(TEXT(OPENGL_LIBRARY_NAME));
	if (!opengl_handle) {
		CUSTOM_ASSERT(false, "failed to load " OPENGL_LIBRARY_NAME);
		return;
	}

	wgl.instance = opengl_handle;
	load_opengl_functions();
	load_extensions(dummy_hdc);
}

static void platform_create_context(HDC hdc, HGLRC share_hrc, Context_Settings settings, Pixel_Format pf_hint) {
	if (settings.profile_api == 0) {
		if (settings.forward && !wgl.ARB_create_context) {
			CUSTOM_ASSERT(false, "forward compatible OpenGL context requires 'ARB_create_context'");
			return;
		}

		if (settings.profile_bit && !wgl.ARB_create_context_profile) {
			CUSTOM_ASSERT(false, "OpenGL profile requires 'ARB_create_context_profile'");
			return;
		}
	}
	else if (settings.profile_api == 1) {
		if (!wgl.EXT_create_context_es2_profile) {
			CUSTOM_ASSERT(false, "OpenGL ES requires 'EXT_create_context_es2_profile'");
			return;
		}
	}

	int pixel_format_id = get_pixel_format(hdc, pf_hint, settings);

	PIXELFORMATDESCRIPTOR pfd;
	if (!DescribePixelFormat(hdc, pixel_format_id, sizeof(pfd), &pfd)) {
		LOG_LAST_ERROR();
		CUSTOM_ASSERT(false, "failed to describe pixel format %d", pixel_format_id);
		return;
	}

	// If hdc references a window, calling the SetPixelFormat function also changes the pixel format of the window. Setting the pixel format of a window more than once can lead to significant complications for the Window Manager and for multithread applications, so it is not allowed. An application can only set the pixel format of a window one time. Once a window's pixel format is set, it cannot be changed.
	if (!SetPixelFormat(hdc, pixel_format_id, &pfd)) {
		LOG_LAST_ERROR();
		CUSTOM_ASSERT(false, "failed to set pixel format %d", pixel_format_id);
		return;
	}

	// You should select a pixel format in the device context before calling the wglCreateContext function. The wglCreateContext function creates a rendering context for drawing on the device in the selected pixel format of the device context.
	HGLRC hrc;
	if (wgl.ARB_create_context) {
		hrc = create_context_arb(hdc, share_hrc, settings);
	}
	else {
		hrc = create_context_legacy(hdc, share_hrc);
	}

	if (hrc && !wgl.MakeCurrent(hdc, hrc)) {
		CUSTOM_ASSERT(false, "failed to make context current");
	}
}

static void platform_shutdown() {
	FreeLibrary(wgl.instance);
}

static void platform_swap_interval(uptr display, HDC hdc, s32 value) {
	if (wgl.EXT_swap_control) {
		wgl.SwapIntervalEXT(value);
	}
}

static void platform_swap_buffers(uptr display, HDC hdc) {
	SwapBuffers(hdc);
}
