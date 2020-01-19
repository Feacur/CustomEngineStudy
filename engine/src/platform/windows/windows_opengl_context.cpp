#include "custom_pch.h"
#include "engine/debug/log.h"
#include "engine/client_api/graphics_context_settings.h"
#include "platform/opengl_context.h"

#include "wgl_tiny.h"

// https://www.khronos.org/registry/OpenGL/extensions/ARB/WGL_ARB_pixel_format.txt
// https://docs.microsoft.com/en-us/windows/win32/api/wingdi/ns-wingdi-pixelformatdescriptor
// https://mariuszbartosik.com/opengl-4-x-initialization-in-windows-without-a-framework/

// https://github.com/glfw/glfw/blob/master/src/wgl_context.c
// https://github.com/spurious/SDL-mirror/blob/master/src/video/windows/SDL_windowsopengl.c
// https://github.com/SFML/SFML/blob/master/src/SFML/Window/Win32/WglContext.cpp

#define OPENGL_LIBRARY_NAME "opengl32.dll"

struct Wgl_Context
{
	HINSTANCE instance;

	// OGL functions
	CreateContext_func     * CreateContext;
	DeleteContext_func     * DeleteContext;
	GetProcAddress_func    * GetProcAddress;
	MakeCurrent_func       * MakeCurrent;
	ShareLists_func        * ShareLists;
	// GetCurrentDC_func      * GetCurrentDC;
	// GetCurrentContext_func * GetCurrentContext;

	// EXT functions
	GetExtensionsStringEXT_func    * GetExtensionsStringEXT;
	SwapIntervalEXT_func           * SwapIntervalEXT;
	// GetSwapIntervalEXT_func        * GetSwapIntervalEXT;

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

	// current pixel_format
	custom::Pixel_Format pixel_format;
	PIXELFORMATDESCRIPTOR pfd;
};

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

// @Todo: put into rendering context object?
static Wgl_Context wgl;

static void * wgl_get_proc_address(cstring name);
static void platform_init_wgl(void);
static HGLRC platform_create_context(HDC hdc, HGLRC share_hrc, custom::Context_Settings * settings, custom::Pixel_Format * hint);
static bool platform_swap_interval(HDC hdc, s32);
static void platform_swap_buffers(HDC hdc, bool doublebuffer);

namespace custom
{
	Opengl_Context::Opengl_Context(uptr hdc, custom::Context_Settings * settings, custom::Pixel_Format * hint)
		: m_hdc(hdc)
		, m_hrc(NULL)
	{
		platform_init_wgl();

		m_hrc = (uptr)platform_create_context((HDC)hdc, NULL, settings, hint);

		// https://docs.microsoft.com/ru-ru/windows/win32/api/wingdi/nf-wingdi-wglgetprocaddress
		int glad_status = gladLoadGLLoader((GLADloadproc)wgl_get_proc_address);
		LOG_LAST_ERROR();
		CUSTOM_ASSERT(glad_status, "failed to initialize glad");
	}

	Opengl_Context::~Opengl_Context()
	{
		wgl.MakeCurrent(NULL, NULL);
		wgl.DeleteContext((HGLRC)m_hrc);
		FreeLibrary(wgl.instance);
	}
	
	void Opengl_Context::set_vsync(s32 value)
	{
		m_is_vsync = platform_swap_interval((HDC)m_hdc, value);
	}

	void Opengl_Context::swap_buffers()
	{
		platform_swap_buffers((HDC)m_hdc, wgl.pixel_format.doublebuffer);
	}
}

//
// platform implementation
//

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
	// LOAD_OPENGL_FUNCTION(GetCurrentDC,   true);
	// LOAD_OPENGL_FUNCTION(GetCurrentContext, true);
}
#undef LOAD_OPENGL_FUNCTION

#define LOAD_EXTENSION_FUNCTION(name) wgl.name = (name##_func *)wgl.GetProcAddress("wgl" #name)
static void load_extension_functions() {
	// EXT functions
	LOAD_EXTENSION_FUNCTION(GetExtensionsStringEXT);
	LOAD_EXTENSION_FUNCTION(SwapIntervalEXT);
	// LOAD_EXTENSION_FUNCTION(GetSwapIntervalEXT);
	// ARB extensions
	LOAD_EXTENSION_FUNCTION(GetExtensionsStringARB);
	LOAD_EXTENSION_FUNCTION(CreateContextAttribsARB);
	LOAD_EXTENSION_FUNCTION(GetPixelFormatAttribivARB);
	// LOAD_EXTENSION_FUNCTION(ChoosePixelFormatARB);
}
#undef LOAD_EXTENSION_FUNCTION

#define CHECK_EXTENSION(name) wgl.name = contains_full_word(extensions_string, "WGL_" #name)
static void check_extension(HDC hdc) {
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

static void load_extensions(HDC hdc) {
	PIXELFORMATDESCRIPTOR pfd = {};
	pfd.nSize        = sizeof(pfd);
	pfd.nVersion     = 1;
	pfd.dwFlags      = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType   = PFD_TYPE_RGBA;
	pfd.cColorBits   = 8 * 3;
	// pfd.cDepthBits   = 8 * 3;
	// pfd.cStencilBits = 8 * 1;

	int pf_id = ChoosePixelFormat(hdc, &pfd);
	if (!pf_id) {
		LOG_LAST_ERROR();
		CUSTOM_ASSERT(false, "failed to describe pixel format");
		return;
	}

	if (!SetPixelFormat(hdc, pf_id, &pfd)) {
		LOG_LAST_ERROR();
		CUSTOM_ASSERT(false, "failed to set pixel format");
		return;
	}

	HGLRC hrc = wgl.CreateContext(hdc);
	if (!hrc) {
		CUSTOM_ASSERT(false, "failed to create rendering context");
		return;
	}

	if (!wgl.MakeCurrent(hdc, hrc)) {
		CUSTOM_ASSERT(false, "failed to make rendering context the current one");
	}
	else {
		load_extension_functions();
		check_extension(hdc);
	}

	wgl.MakeCurrent(NULL, NULL);
	wgl.DeleteContext(hrc);
}

#define ADD_ATTRIBUTE_KEY(key) {\
	CUSTOM_ASSERT(count < cap, "attributes capacity reached");\
	keys[count++] = key;\
}
static int add_atribute_keys(int * keys, int cap, custom::Context_Settings * settings) {
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

	if (settings->api == custom::Context_Api::OpenGL) {
		if (wgl.ARB_framebuffer_sRGB || wgl.EXT_framebuffer_sRGB) {
			ADD_ATTRIBUTE_KEY(WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB);
		}
	}
	else if (settings->api == custom::Context_Api::OpenGL_ES) {
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
static custom::Pixel_Format * allocate_pixel_formats_arb(HDC hdc, custom::Context_Settings * settings) {
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
	CREATE_ARRAY(custom::Pixel_Format, formats_count, pf_list);
	for (int i = 0; i < formats_count; ++i) {
		int pf_id = i + 1;
		if (!wgl.GetPixelFormatAttribivARB(hdc, pf_id, 0, attr_count, attr_keys, attr_vals)) {
			CUSTOM_WARN("failed to get pixel format %d values", pf_id);
			continue;
		}
		// should support
		if (!GET_ATTRIBUTE_VALUE(WGL_DRAW_TO_WINDOW_ARB)) { continue; }
		if (!GET_ATTRIBUTE_VALUE(WGL_SUPPORT_OPENGL_ARB)) { continue; }
		if (GET_ATTRIBUTE_VALUE(WGL_PIXEL_TYPE_ARB) != WGL_TYPE_RGBA_ARB) {
			continue;
		}
		if (settings->api == custom::Context_Api::OpenGL) {
			if (wgl.ARB_framebuffer_sRGB || wgl.EXT_framebuffer_sRGB) {
				if (!GET_ATTRIBUTE_VALUE(WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB)) {
					continue;
				}
			}
		}
		else if (settings->api == custom::Context_Api::OpenGL_ES) {
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

		custom::Pixel_Format pf = {};
		pf.id = pf_id;
		pf.redBits      = GET_ATTRIBUTE_VALUE(WGL_RED_BITS_ARB);
		pf.greenBits    = GET_ATTRIBUTE_VALUE(WGL_GREEN_BITS_ARB);
		pf.blueBits     = GET_ATTRIBUTE_VALUE(WGL_BLUE_BITS_ARB);
		pf.alphaBits    = GET_ATTRIBUTE_VALUE(WGL_ALPHA_BITS_ARB);
		pf.depthBits    = GET_ATTRIBUTE_VALUE(WGL_DEPTH_BITS_ARB);
		pf.stencilBits  = GET_ATTRIBUTE_VALUE(WGL_STENCIL_BITS_ARB);
		pf.doublebuffer = GET_ATTRIBUTE_VALUE(WGL_DOUBLE_BUFFER_ARB);

		pf_list[pf_count++] = pf;

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
		// pfa.transparent = GET_ATTRIBUTE_VALUE(WGL_TRANSPARENT_ARB);
		*/
	}

	pf_list[pf_count].id = NULL;
	return pf_list;
}
#undef GET_ATTRIBUTE_VALUE

static custom::Pixel_Format * allocate_pixel_formats_legacy(HDC hdc) {
	int formats_count = DescribePixelFormat(
		hdc, 1, sizeof(PIXELFORMATDESCRIPTOR), NULL
	);
	LOG_LAST_ERROR();
	if (!formats_count) { return NULL; }

	int pf_count = 0;
	CREATE_ARRAY(custom::Pixel_Format, formats_count, pf_list);
	for (int i = 0; i < formats_count; ++i) {
		int pf_id = i + 1;
		PIXELFORMATDESCRIPTOR pfd;
		if (!DescribePixelFormat(hdc, pf_id, sizeof(pfd), &pfd))
		{
			LOG_LAST_ERROR();
			CUSTOM_WARN("failed to describe pixel format %d", pf_id);
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

		custom::Pixel_Format pf = {};
		pf.id = pf_id;
		pf.redBits      = pfd.cRedBits;
		pf.greenBits    = pfd.cGreenBits;
		pf.blueBits     = pfd.cBlueBits;
		pf.alphaBits    = pfd.cAlphaBits;
		pf.depthBits    = pfd.cDepthBits;
		pf.stencilBits  = pfd.cStencilBits;
		pf.doublebuffer = bits_are_set(pfd.dwFlags, PFD_DOUBLEBUFFER);

		pf_list[pf_count++] = pf;

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
		// pfa.transparent = true;
		*/
	}

	pf_list[pf_count].id = NULL;
	return pf_list;
}

static int find_best_pixel_format(custom::Pixel_Format * list, custom::Pixel_Format * hint) {
	custom::Pixel_Format local_hint = *hint;
	custom::Pixel_Format best_match = {};
	for (custom::Pixel_Format * pf = list; pf && pf->id; ++pf)
	{
		if (pf->doublebuffer != local_hint.doublebuffer) { continue; }

		if (pf->redBits     < local_hint.redBits)     { continue; }
		if (pf->greenBits   < local_hint.greenBits)   { continue; }
		if (pf->blueBits    < local_hint.blueBits)    { continue; }
		if (pf->alphaBits   < local_hint.alphaBits)   { continue; }
		if (pf->depthBits   < local_hint.depthBits)   { continue; }
		if (pf->stencilBits < local_hint.stencilBits) { continue; }

		// @Todo: implement/take better algorithm
		best_match = *pf;
		break;
	}
	*hint = best_match;
	return (int)best_match.id;
}

// @Note: might well used built-in ChoosePixelFormatARB(...), too
static int get_pixel_format(HDC hdc, custom::Context_Settings * settings, custom::Pixel_Format * hint) {
	custom::Pixel_Format * list;
	if (wgl.ARB_pixel_format) {
		list = allocate_pixel_formats_arb(hdc, settings);
	}
	else {
		list = allocate_pixel_formats_legacy(hdc);
	}

	if (!list) { return 0; }
	int pf_id = find_best_pixel_format(list, hint);
	free(list);

	return pf_id;
}

#define SET_ATTRIBUTE(key, value) {\
	CUSTOM_ASSERT(attr_count < attr_cap, "attributes capacity reached");\
	attr_pair[attr_count++] = key;\
	attr_pair[attr_count++] = value;\
}
static HGLRC create_context_arb(HDC hdc, HGLRC share_hrc, custom::Context_Settings * settings) {
	int const attr_cap = 64 * 2;
	int attr_pair[attr_cap] = {};
	int attr_count = 0;

	int profile_mask = 0;
	int context_flags = 0;

	if (settings->api == custom::Context_Api::OpenGL) {
		if (settings->forward) {
			context_flags |= WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB;
		}
		if (settings->profile_bit) {
			profile_mask |= settings->profile_bit;
		}
	}
	else if (settings->api == custom::Context_Api::OpenGL_ES) {
		profile_mask |= WGL_CONTEXT_ES2_PROFILE_BIT_EXT;
	}

	if (settings->major_version != 1 || settings->minor_version != 0) {
		SET_ATTRIBUTE(WGL_CONTEXT_MAJOR_VERSION_ARB, settings->major_version);
		SET_ATTRIBUTE(WGL_CONTEXT_MINOR_VERSION_ARB, settings->minor_version);
	}

	if (settings->robustness && wgl.ARB_create_context_robustness) {
		SET_ATTRIBUTE(
			WGL_CONTEXT_RESET_NOTIFICATION_STRATEGY_ARB,
			settings->robustness
		);
		context_flags |= WGL_CONTEXT_ROBUST_ACCESS_BIT_ARB;
	}

	if (settings->release_behaviour && wgl.ARB_context_flush_control) {
		SET_ATTRIBUTE(
			WGL_CONTEXT_RELEASE_BEHAVIOR_ARB,
			settings->release_behaviour
		);
	}

	if (settings->debug) {
		context_flags |= WGL_CONTEXT_DEBUG_BIT_ARB;
	}

	if (settings->opengl_no_error && wgl.ARB_create_context_no_error) {
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

static HWND create_dummy_window(void) {
	DWORD     dwExStyle  = WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR;
	DWORD     dwStyle    = WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
	HWND      hWndParent = HWND_DESKTOP;
	HMENU     hMenu      = NULL;
	HINSTANCE hInstance  = GetModuleHandle(NULL);
	LPVOID    lpParam    = NULL;

	// https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-createwindowexa
	HWND hwnd = CreateWindowEx(
		dwExStyle,
		// @Note: match with the core window class name
		TEXT("custom engine"), TEXT(""),
		dwStyle,
		// int X, Y, nWidth, nHeight
		0, 0, 1, 1,
		hWndParent, hMenu, hInstance, lpParam
	);
	CUSTOM_ASSERT(hwnd, "failed to create window");
	return hwnd;
}

static void platform_init_wgl(void) {
	HINSTANCE opengl_handle = LoadLibrary(TEXT(OPENGL_LIBRARY_NAME));
	if (!opengl_handle) {
		CUSTOM_ASSERT(false, "failed to load " OPENGL_LIBRARY_NAME);
		return;
	}

	wgl.instance = opengl_handle;
	load_opengl_functions();

	// @Note: should a dummy window class be created here, too?

	// @Note: the reason behind this dummy window
	// https://docs.microsoft.com/ru-ru/windows/win32/api/wingdi/nf-wingdi-setpixelformat
	// If hdc references a window, calling the SetPixelFormat function also changes the pixel format of the window. Setting the pixel format of a window more than once can lead to significant complications for the Window Manager and for multithread applications, so it is not allowed. An application can only set the pixel format of a window one time. Once a window's pixel format is set, it cannot be changed.
	// You should select a pixel format in the device context before calling the wglCreateContext function. The wglCreateContext function creates a rendering context for drawing on the device in the selected pixel format of the device context.
	// An OpenGL window has its own pixel format. Because of this, only device contexts retrieved for the client area of an OpenGL window are allowed to draw into the window. As a result, an OpenGL window should be created with the WS_CLIPCHILDREN and WS_CLIPSIBLINGS styles. Additionally, the window class attribute should not include the CS_PARENTDC style.
	HWND hwnd = create_dummy_window();
	HDC hdc = GetDC(hwnd);

	load_extensions(hdc);

	// @Note: is ReleaseDC necessary here?
	ReleaseDC(hwnd, hdc);
	DestroyWindow(hwnd);
}

static HGLRC platform_create_context(HDC hdc, HGLRC share_hrc, custom::Context_Settings * settings, custom::Pixel_Format * hint) {
	if (settings->api == custom::Context_Api::OpenGL) {
		if (settings->forward && !wgl.ARB_create_context) {
			CUSTOM_ASSERT(false, "forward compatible OpenGL context requires 'ARB_create_context'");
			return NULL;
		}

		if (settings->profile_bit && !wgl.ARB_create_context_profile) {
			CUSTOM_ASSERT(false, "OpenGL profile requires 'ARB_create_context_profile'");
			return NULL;
		}
	}
	else if (settings->api == custom::Context_Api::OpenGL_ES) {
		if (!wgl.EXT_create_context_es2_profile) {
			CUSTOM_ASSERT(false, "OpenGL ES requires 'EXT_create_context_es2_profile'");
			return NULL;
		}
	}

	int pf_id = get_pixel_format(hdc, settings, hint);
	wgl.pixel_format = *hint;

	PIXELFORMATDESCRIPTOR pfd;
	if (!DescribePixelFormat(hdc, pf_id, sizeof(pfd), &pfd)) {
		LOG_LAST_ERROR();
		CUSTOM_ASSERT(false, "failed to describe pixel format %d", pf_id);
		return NULL;
	}
	wgl.pfd = pfd;

	if (!SetPixelFormat(hdc, pf_id, &pfd)) {
		LOG_LAST_ERROR();
		CUSTOM_ASSERT(false, "failed to set pixel format %d", pf_id);
		return NULL;
	}

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

	return hrc;
}

static bool platform_swap_interval(HDC hdc, s32 value) {
	if (wgl.EXT_swap_control) {
		return wgl.SwapIntervalEXT(value);
	}
	return false;
}

static void platform_swap_buffers(HDC hdc, bool doublebuffer) {
	// https://www.khronos.org/opengl/wiki/Swap_Interval
	// https://www.khronos.org/opengl/wiki/Common_Mistakes#glFinish_and_glFlush
	if (wgl.pixel_format.doublebuffer) {
		SwapBuffers(hdc);
	}
	else {
		glFlush();
	}
}
