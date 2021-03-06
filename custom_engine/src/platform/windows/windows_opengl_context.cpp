#include "custom_pch.h"

#include "platform/graphics_context.h"

#include "engine/api/rendering_settings.h"
#include "engine/core/code.h"
#include "engine/debug/log.h"

#if !defined(CUSTOM_PRECOMPILED_HEADER)
	#include <Windows.h>
	#include <glad/glad.h>
#endif

#include "wgl_tiny.h"
#include "platform/opengl/extensions_loader.h"

#define ALLOCATE_ARRAY(name, type, count) type * name = (type *)malloc(count * sizeof(type))

// https://en.wikipedia.org/wiki/OpenGL
// https://www.khronos.org/registry/OpenGL/extensions/ARB/WGL_ARB_pixel_format.txt
// https://docs.microsoft.com/en-us/windows/win32/api/wingdi/ns-wingdi-pixelformatdescriptor
// https://mariuszbartosik.com/opengl-4-x-initialization-in-windows-without-a-framework/

// https://www.glfw.org/docs/latest/context_guide.html

// https://github.com/glfw/glfw/blob/master/src/wgl_context.c
// https://github.com/spurious/SDL-mirror/blob/master/src/video/windows/SDL_windowsopengl.c
// https://github.com/SFML/SFML/blob/master/src/SFML/Window/Win32/WglContext.cpp
// https://github.com/HandmadeHero/cpp/blob/master/code/win32_handmade_opengl.cpp

// https://www.khronos.org/opengl/wiki/OpenGL_Context
// A context stores all of the state associated with this instance of OpenGL.
// Think of a context as an object that holds all of OpenGL;
// when a context is destroyed, OpenGL is destroyed.
// In order for any OpenGL commands to work, a context must be current

// https://www.khronos.org/opengl/wiki/Creating_an_OpenGL_Context_(WGL)
// * OpenGL context creation is governed by platform-specific APIs.
// * When you create your HWND, you need to make sure that it has the CS_OWNDC set for its style.
// * The function you use to get WGL extensions is, itself, an OpenGL extension.
//   All we need to do is create a dummy context to get function pointers.
//   Windows does not allow the user to change the pixel format of a window. You get to set it exactly once.
//   Therefore, if you want to use a different pixel format from the one your fake context used (for sRGB or multisample framebuffers, or just different bit-depths of buffers), you must destroy the window entirely and recreate it after we are finished with the dummy context.

#define OPENGL_LIBRARY_NAME "opengl32.dll"

// @Todo: dynamically allocate upon creation?
//        incorporate into the Internal_Data?
static struct {
	HINSTANCE instance;

	// WGL functions
	CreateContext_func     * CreateContext;
	DeleteContext_func     * DeleteContext;
	GetProcAddress_func    * GetProcAddress;
	MakeCurrent_func       * MakeCurrent;
	ShareLists_func        * ShareLists;
	// SwapLayerBuffers_func  * SwapLayerBuffers;
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
} wgl;

// #include "engine/math/bitwise.h"
constexpr static inline bool bits_are_set(DWORD container, DWORD bits) {
	return (container & bits) == bits;
}

#if !defined(CUSTOM_SHIPPING)
	void log_last_error(cstring source);
	#define LOG_LAST_ERROR() log_last_error(CUSTOM_FILE_AND_LINE)
#else
	#define LOG_LAST_ERROR() (void)0
#endif

//
// API implementation
//

static void * load_ogl_function(cstring name);
static void platform_init_wgl(void);
static HGLRC platform_create_context(HDC hdc, HGLRC share_hrc);
static bool platform_swap_interval(HDC hdc, u32 value);

namespace custom {
namespace window {
	// @Forward
	HDC get_hdc(Internal_Data * window);
}}

namespace custom {
namespace context {

struct Internal_Data
{
	HDC hdc;
	HGLRC hrc;
	bool is_vsync;
};

Internal_Data * create(window::Internal_Data * window) {
	switch (custom::context_settings.major_version)
	{
		case 0:
			CUSTOM_ASSERT(custom::context_settings.minor_version == 0, "wrong default version"); break;
		case 1:
			CUSTOM_ASSERT(custom::context_settings.minor_version <= 5, "wrong 1.x version"); break;
		case 2:
			CUSTOM_ASSERT(custom::context_settings.minor_version <= 1, "wrong 2.x version"); break;
		case 3:
			CUSTOM_ASSERT(custom::context_settings.minor_version <= 3, "wrong 3.x version"); break;
		case 4:
			CUSTOM_ASSERT(custom::context_settings.minor_version <= 6, "wrong 4.x version (?)"); break;
		default:
			CUSTOM_ASSERT(false, "unhandled version"); break;
	}

	platform_init_wgl();

	Internal_Data * data = (Internal_Data *)calloc(1, sizeof(Internal_Data));
	CUSTOM_ASSERT(data, "failed to allocate context");

	data->hdc = window::get_hdc(window);
	data->hrc = platform_create_context(data->hdc, NULL);

	// https://docs.microsoft.com/ru-ru/windows/win32/api/wingdi/nf-wingdi-wglgetprocaddress
	int glad_status = gladLoadGLLoader(load_ogl_function);
	// @Note: actually logs some errors, but still works? even for GLFW?!
	//        seems like OpenGL 4.6 reports something for unknown reasons
	LOG_LAST_ERROR();
	CUSTOM_ASSERT(glad_status, "failed to initialize glad");

	opengl::load_extensions(load_ogl_function);

	CUSTOM_TRACE(
		"OpenGL info:"
		ANSI_TXT_GRY "\n  - vendor:   " ANSI_TXT_GRN "%s" ANSI_CLR
		ANSI_TXT_GRY "\n  - renderer: " ANSI_TXT_GRN "%s" ANSI_CLR
		ANSI_TXT_GRY "\n  - version:  " ANSI_TXT_GRN "%s" ANSI_CLR
		ANSI_TXT_GRY "\n  - shaders:  " ANSI_TXT_GRN "%s" ANSI_CLR,
		glGetString(GL_VENDOR),
		glGetString(GL_RENDERER),
		glGetString(GL_VERSION),
		glGetString(GL_SHADING_LANGUAGE_VERSION)
	);

	return data;
}

void destroy(Internal_Data * data) {
	CUSTOM_ASSERT(data->hdc, "context doesn't exist");
	if (data->hrc) {
		wgl.MakeCurrent(NULL, NULL); LOG_LAST_ERROR();
		wgl.DeleteContext(data->hrc); LOG_LAST_ERROR();
		data->hrc = NULL;
	}
	FreeLibrary(wgl.instance); LOG_LAST_ERROR();
	free(data);
	ZeroMemory(&wgl, sizeof(wgl));
}

void set_vsync(Internal_Data * data, u8 value) {
	CUSTOM_ASSERT(data->hdc, "context doesn't exist");
	if (wgl.pixel_format.doublebuffer) {
		if (platform_swap_interval(data->hdc, value)) {
			data->is_vsync = value > 0;
		}
		else {
			LOG_LAST_ERROR();
			CUSTOM_WARNING("failed to set vsync");
		}
	}
}

bool check_vsync(Internal_Data * data) {
	CUSTOM_ASSERT(data->hdc, "context doesn't exist");
	return data->is_vsync;
}

void swap_buffers(Internal_Data * data) {
	CUSTOM_ASSERT(data->hdc, "context doesn't exist");
	if (wgl.pixel_format.doublebuffer) {
		if (SwapBuffers(data->hdc)) { return; }
		// if (wgl.SwapLayerBuffers(data->hdc, WGL_SWAP_MAIN_PLANE)) { return; }
	}

	// @Todo: whether `glFinish` might be helpful?
	// glFinish();

	// @Todo: whether `glFlush` is even required?
	glFlush();
}

}}

//
// platform implementation
//

static void * load_ogl_function(cstring name) {
	if (!name) { return NULL; }
	void * address = (void *)wgl.GetProcAddress(name);
	if (!address) {
		address = (void *)GetProcAddress(wgl.instance, name);
	}
	// if (!address) { CUSTOM_WARNING("missing an OpenGL function: %s", name); }
	return address;
}

#define LOAD_FUNCTION(name, required) {                                                              \
    wgl.name = (name##_func *)GetProcAddress(wgl.instance, "wgl" #name);                             \
    CUSTOM_ASSERT(wgl.name || !required, "failed to load 'wgl" #name "' from " OPENGL_LIBRARY_NAME); \
} while (0)                                                                                          \

static void load_wgl_simple_functions(void) {
	LOAD_FUNCTION(CreateContext,  true);
	LOAD_FUNCTION(DeleteContext,  true);
	LOAD_FUNCTION(GetProcAddress, true);
	LOAD_FUNCTION(MakeCurrent,    true);
	LOAD_FUNCTION(ShareLists,     true);
	// LOAD_FUNCTION(SwapLayerBuffers, true);
	// LOAD_FUNCTION(GetCurrentDC,   true);
	// LOAD_FUNCTION(GetCurrentContext, true);
}
#undef LOAD_FUNCTION

#define LOAD_FUNCTION(name) wgl.name = (name##_func *)wgl.GetProcAddress("wgl" #name)
static void load_wgl_extension_functions(void) {
	// EXT functions
	LOAD_FUNCTION(GetExtensionsStringEXT);
	LOAD_FUNCTION(SwapIntervalEXT);
	// LOAD_FUNCTION(GetSwapIntervalEXT);
	// ARB extensions
	LOAD_FUNCTION(GetExtensionsStringARB);
	LOAD_FUNCTION(CreateContextAttribsARB);
	LOAD_FUNCTION(GetPixelFormatAttribivARB);
	// LOAD_FUNCTION(ChoosePixelFormatARB);
}
#undef LOAD_FUNCTION

#define CHECK_EXTENSION(name) wgl.name = opengl::contains_full_word(extensions_string, "WGL_" #name)
static void load_extension_flags(HDC hdc) {
	if (wgl.GetExtensionsStringEXT) {
		cstring extensions_string = wgl.GetExtensionsStringEXT();
		CHECK_EXTENSION(EXT_framebuffer_sRGB);
		CHECK_EXTENSION(EXT_create_context_es2_profile);
		CHECK_EXTENSION(EXT_swap_control);
		CHECK_EXTENSION(EXT_swap_control_tear);
		CHECK_EXTENSION(EXT_colorspace);
	}
	else {
		CUSTOM_WARNING("failed to load EXT extensions string");
	}

	if (wgl.GetExtensionsStringARB) {
		cstring extensions_string = wgl.GetExtensionsStringARB(hdc);
		CHECK_EXTENSION(ARB_multisample);
		CHECK_EXTENSION(ARB_framebuffer_sRGB);
		CHECK_EXTENSION(ARB_create_context_robustness);
		CHECK_EXTENSION(ARB_create_context_no_error);
		CHECK_EXTENSION(ARB_pixel_format);
		CHECK_EXTENSION(ARB_context_flush_control);
		CHECK_EXTENSION(ARB_create_context);
		CHECK_EXTENSION(ARB_create_context_profile);
	}
	else {
		CUSTOM_WARNING("failed to load ARB extensions string");
	}
}
#undef CHECK_EXTENSION

static void load_extensions(HDC hdc) {
	PIXELFORMATDESCRIPTOR pfd; // = {};
	ZeroMemory(&pfd, sizeof(pfd));
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

	HGLRC hrc = wgl.CreateContext(hdc); LOG_LAST_ERROR();
	if (!hrc) {
		CUSTOM_ASSERT(false, "failed to create rendering context");
		return;
	}

	if (wgl.MakeCurrent(hdc, hrc)) {
		load_wgl_extension_functions();
		load_extension_flags(hdc);
	}
	else {
		CUSTOM_ASSERT(false, "failed to make rendering context the current one");
	} LOG_LAST_ERROR();

	wgl.MakeCurrent(NULL, NULL); LOG_LAST_ERROR();
	wgl.DeleteContext(hrc); LOG_LAST_ERROR();
}

#define ADD_ATTRIBUTE_KEY(key) do {                            \
    CUSTOM_ASSERT(count < cap, "attributes capacity reached"); \
    keys[count++] = key;                                       \
} while (0);                                                   \

static int add_atribute_keys(int * keys, int cap) {
	ZeroMemory(keys, cap * sizeof(*keys));
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

	if (custom::context_settings.api == custom::Context_Api::OpenGL) {
		if (wgl.ARB_framebuffer_sRGB || wgl.EXT_framebuffer_sRGB) {
			ADD_ATTRIBUTE_KEY(WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB);
		}
	}
	else if (custom::context_settings.api == custom::Context_Api::OpenGL_ES) {
		if (wgl.EXT_colorspace) {
			ADD_ATTRIBUTE_KEY(WGL_COLORSPACE_EXT);
		}
	}

	if (custom::pixel_format_hint.doublebuffer) {
		ADD_ATTRIBUTE_KEY(WGL_SWAP_METHOD_ARB);
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
static custom::Pixel_Format * allocate_pixel_formats_arb(HDC hdc) {
	// The number of pixel formats for the device context.
	// The <iLayerPlane> and <iPixelFormat> parameters are ignored
	int const formats_request = WGL_NUMBER_PIXEL_FORMATS_ARB;
	int formats_count;
	if (!wgl.GetPixelFormatAttribivARB(hdc, 0, 0, 1, &formats_request, &formats_count)) {
		LOG_LAST_ERROR();
		CUSTOM_ASSERT(false, "failed to count pixel formats");
		return NULL;
	}
	if (!formats_count) { return NULL; }

	int const attr_cap = 64;
	int attr_keys[attr_cap]; // = {};
	int attr_vals[attr_cap];
	int attr_count = add_atribute_keys(attr_keys, attr_cap);

	int pf_count = 0;
	ALLOCATE_ARRAY(pf_list, custom::Pixel_Format, formats_count);
	for (int i = 0; i < formats_count; ++i) {
		int pf_id = i + 1;
		if (!wgl.GetPixelFormatAttribivARB(hdc, pf_id, 0, attr_count, attr_keys, attr_vals)) {
			LOG_LAST_ERROR();
			CUSTOM_WARNING("failed to get pixel format %d values", pf_id);
			continue;
		}
		// should support
		if (!GET_ATTRIBUTE_VALUE(WGL_DRAW_TO_WINDOW_ARB)) { continue; }
		if (!GET_ATTRIBUTE_VALUE(WGL_SUPPORT_OPENGL_ARB)) { continue; }
		if (GET_ATTRIBUTE_VALUE(WGL_PIXEL_TYPE_ARB) != WGL_TYPE_RGBA_ARB) {
			continue;
		}
		if (custom::context_settings.api == custom::Context_Api::OpenGL) {
			if (wgl.ARB_framebuffer_sRGB || wgl.EXT_framebuffer_sRGB) {
				if (!GET_ATTRIBUTE_VALUE(WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB)) {
					continue;
				}
			}
		}
		else if (custom::context_settings.api == custom::Context_Api::OpenGL_ES) {
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

		custom::Pixel_Format pf; // = {};
		ZeroMemory(&pf, sizeof(pf));
		pf.id = pf_id;
		pf.red_bits     = GET_ATTRIBUTE_VALUE(WGL_RED_BITS_ARB);
		pf.green_bits   = GET_ATTRIBUTE_VALUE(WGL_GREEN_BITS_ARB);
		pf.blue_bits    = GET_ATTRIBUTE_VALUE(WGL_BLUE_BITS_ARB);
		pf.alpha_bits   = GET_ATTRIBUTE_VALUE(WGL_ALPHA_BITS_ARB);
		pf.depth_bits   = GET_ATTRIBUTE_VALUE(WGL_DEPTH_BITS_ARB);
		pf.stencil_bits = GET_ATTRIBUTE_VALUE(WGL_STENCIL_BITS_ARB);
		pf.doublebuffer = GET_ATTRIBUTE_VALUE(WGL_DOUBLE_BUFFER_ARB);
		pf.swap = !pf.doublebuffer ? 0
			: (GET_ATTRIBUTE_VALUE(WGL_SWAP_METHOD_ARB) != WGL_SWAP_EXCHANGE_ARB) ? 1
			: (GET_ATTRIBUTE_VALUE(WGL_SWAP_METHOD_ARB) != WGL_SWAP_COPY_ARB)     ? 2
			: 0;

		pf_list[pf_count++] = pf;

		/*
		custom::Pixel_Format_Aux pfa; // = {};
		ZeroMemory(&pfa, sizeof(pfa));
		pfa.red_shift   = GET_ATTRIBUTE_VALUE(WGL_RED_SHIFT_ARB);
		pfa.green_shift = GET_ATTRIBUTE_VALUE(WGL_GREEN_SHIFT_ARB);
		pfa.blue_shift  = GET_ATTRIBUTE_VALUE(WGL_BLUE_SHIFT_ARB);
		pfa.alpha_shift = GET_ATTRIBUTE_VALUE(WGL_ALPHA_SHIFT_ARB);
		//
		pfa.accum_red_bits   = GET_ATTRIBUTE_VALUE(WGL_ACCUM_RED_BITS_ARB);
		pfa.accum_green_bits = GET_ATTRIBUTE_VALUE(WGL_ACCUM_GREEN_BITS_ARB);
		pfa.accum_blue_bits  = GET_ATTRIBUTE_VALUE(WGL_ACCUM_BLUE_BITS_ARB);
		pfa.accum_alpha_bits = GET_ATTRIBUTE_VALUE(WGL_ACCUM_ALPHA_BITS_ARB);
		//
		pfa.aux_buffers = GET_ATTRIBUTE_VALUE(WGL_AUX_BUFFERS_ARB);
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
	ALLOCATE_ARRAY(pf_list, custom::Pixel_Format, formats_count);
	for (int i = 0; i < formats_count; ++i) {
		int pf_id = i + 1;
		PIXELFORMATDESCRIPTOR pfd;
		if (!DescribePixelFormat(hdc, pf_id, sizeof(pfd), &pfd))
		{
			LOG_LAST_ERROR();
			CUSTOM_WARNING("failed to describe pixel format %d", pf_id);
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

		custom::Pixel_Format pf; // = {};
		ZeroMemory(&pf, sizeof(pf));
		pf.id = pf_id;
		pf.red_bits     = pfd.cRedBits;
		pf.green_bits   = pfd.cGreenBits;
		pf.blue_bits    = pfd.cBlueBits;
		pf.alpha_bits   = pfd.cAlphaBits;
		pf.depth_bits   = pfd.cDepthBits;
		pf.stencil_bits = pfd.cStencilBits;
		pf.doublebuffer = bits_are_set(pfd.dwFlags, PFD_DOUBLEBUFFER);
		pf.swap = !pf.doublebuffer ? 0
			: bits_are_set(pfd.dwFlags, PFD_SWAP_EXCHANGE) ? 1
			: bits_are_set(pfd.dwFlags, PFD_SWAP_COPY)     ? 2
			: 0;

		pf_list[pf_count++] = pf;

		/*
		custom::Pixel_Format_Aux pfa; // = {};
		ZeroMemory(&pfa, sizeof(pfa));
		pfa.red_shift   = pfd.cRedShift;
		pfa.green_shift = pfd.cGreenShift;
		pfa.blue_shift  = pfd.cBlueShift;
		pfa.alpha_shift = pfd.cAlphaShift;
		//
		pfa.accum_red_bits   = pfd.cAccumRedBits;
		pfa.accum_green_bits = pfd.cAccumGreenBits;
		pfa.accum_blue_bits  = pfd.cAccumBlueBits;
		pfa.accum_alpha_bits = pfd.cAccumAlphaBits;
		//
		pfa.aux_buffers = pfd.cAuxBuffers;
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

static inline int get_rgb_distance(custom::Pixel_Format * a, custom::Pixel_Format * b) {
	int v1 = (a->red_bits   - b->red_bits);
	int v2 = (a->green_bits - b->green_bits);
	int v3 = (a->blue_bits  - b->blue_bits);
	return v1 * v1 + v2 * v2 + v3 * v3;
}

static inline int get_ads_distance(custom::Pixel_Format * a, custom::Pixel_Format * b) {
	int v1 = (a->alpha_bits   - b->alpha_bits);
	int v2 = (a->depth_bits   - b->depth_bits);
	int v3 = (a->stencil_bits - b->stencil_bits);
	return v1 * v1 + v2 * v2 + v3 * v3;
}

static custom::Pixel_Format * find_best_pixel_format(custom::Pixel_Format * list) {
	custom::Pixel_Format * match = list;
	int match_dist = get_rgb_distance(match, &custom::pixel_format_hint)
	               + get_ads_distance(match, &custom::pixel_format_hint);
	for (custom::Pixel_Format * pf = list + 1; pf && pf->id; ++pf)
	{
		// @Note: should satisfy minimal requirements
		if (pf->doublebuffer != custom::pixel_format_hint.doublebuffer) { continue; }
		if (custom::pixel_format_hint.swap && pf->swap != custom::pixel_format_hint.swap) { continue; }

		// @Note: might loosen this restrictions
		//        GLFW deems hint as a vague target
		//             * prefers not to miss alpha-depth-stencil bits
		//             * seeks the closest RGB set
		//             * seeks the closest alpha-depth-stencil set
		//        SDL deems hint as the minimum
		//            * seeks the closest
		if (pf->red_bits     < custom::pixel_format_hint.red_bits)     { continue; }
		if (pf->green_bits   < custom::pixel_format_hint.green_bits)   { continue; }
		if (pf->blue_bits    < custom::pixel_format_hint.blue_bits)    { continue; }
		if (pf->alpha_bits   < custom::pixel_format_hint.alpha_bits)   { continue; }
		if (pf->depth_bits   < custom::pixel_format_hint.depth_bits)   { continue; }
		if (pf->stencil_bits < custom::pixel_format_hint.stencil_bits) { continue; }

		// @Note: select the closest
		int pf_dist = get_rgb_distance(pf, &custom::pixel_format_hint)
		            + get_ads_distance(pf, &custom::pixel_format_hint);
		if (pf_dist >= match_dist) { continue; }

		match = pf;
		match_dist = pf_dist;
	}
	// @Todo: probably then seek for bits only if can't match the requested doublebuffer?
	//        and then launch loose GLFW-like heuristic? overkill?
	return match;
}

// @Note: might well used built-in ChoosePixelFormatARB(...), too
static custom::Pixel_Format choose_pixel_format(HDC hdc) {
	custom::Pixel_Format * list;
	if (wgl.ARB_pixel_format) {
		list = allocate_pixel_formats_arb(hdc);
	}
	else {
		list = allocate_pixel_formats_legacy(hdc);
	}

	if (!list) { return {}; }
	custom::Pixel_Format pf = *find_best_pixel_format(list);
	free(list);

	return pf;
}

#define SET_ATTRIBUTE(key, value) do {                                   \
    CUSTOM_ASSERT(attr_count < attr_cap, "attributes capacity reached"); \
    attr_pair[attr_count++] = key;                                       \
    attr_pair[attr_count++] = value;                                     \
} while (0);                                                             \

static HGLRC create_context_arb(HDC hdc, HGLRC share_hrc) {
	int const attr_cap = 64 * 2;
	int attr_pair[attr_cap];
	int attr_count = 0;

	int profile_mask = 0;
	int context_flags = 0;

	if (custom::context_settings.api == custom::Context_Api::OpenGL) {
		// Forward-compatible contexts are defined only for OpenGL versions 3.0 and later. They must not support functionality marked as <deprecated> by that version of the API
		// Feature deprecation was introduced with OpenGL 3.0, so forward-compatible contexts may only be requested for OpenGL 3.0 and above.
		// If attributes WGL_CONTEXT_MAJOR_VERSION_ARB and WGL_CONTEXT_MINOR_VERSION_ARB, when considered together with WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB, specify an OpenGL version and feature set that are not defined, then ERROR_INVALID_VERSION_ARB is generated.
		// Because the purpose of forward-compatible contexts is to allow application development on a specific OpenGL version with the knowledge that the app will run on a future version, context creation will fail if WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB is set and the context version returned cannot implement exactly the requested version.
		if (custom::context_settings.major_version >= 3) {
			if (custom::context_settings.legacy_context) {
				profile_mask |= WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB;
			}
			else {
				context_flags |= WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB;
			}
		}
		//  If the requested OpenGL version is less than 3.2, WGL_CONTEXT_PROFILE_MASK_ARB is ignored and the functionality of the context is determined solely by the requested version.
		if (custom::context_settings.legacy_profile) {
			profile_mask |= WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB;
		}
		else {
			profile_mask |= WGL_CONTEXT_CORE_PROFILE_BIT_ARB;
		}
	}
	else if (custom::context_settings.api == custom::Context_Api::OpenGL_ES) {
		profile_mask |= WGL_CONTEXT_ES2_PROFILE_BIT_EXT;
	}

	if (custom::context_settings.major_version) {
		SET_ATTRIBUTE(WGL_CONTEXT_MAJOR_VERSION_ARB, custom::context_settings.major_version);
		SET_ATTRIBUTE(WGL_CONTEXT_MINOR_VERSION_ARB, custom::context_settings.minor_version);
	}

	if (custom::context_settings.robustness && wgl.ARB_create_context_robustness) {
		// https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_robustness.txt
		// - If the reset notification behavior is NO_RESET_NOTIFICATION_ARB, then the implementation will never deliver notification of reset events, and GetGraphicsResetStatusARB will always return
		// - If the behavior is LOSE_CONTEXT_ON_RESET_ARB, a graphics reset will result in the loss of all context state, requiring the recreation of all associated objects. In this case GetGraphicsResetStatusARB. may return any of the values described above.
		SET_ATTRIBUTE(
			WGL_CONTEXT_RESET_NOTIFICATION_STRATEGY_ARB,
			custom::context_settings.robustness == 1 ? WGL_NO_RESET_NOTIFICATION_ARB : WGL_LOSE_CONTEXT_ON_RESET_ARB
		);
		context_flags |= WGL_CONTEXT_ROBUST_ACCESS_BIT_ARB;
	}

	if (wgl.ARB_context_flush_control) {
		SET_ATTRIBUTE(
			WGL_CONTEXT_RELEASE_BEHAVIOR_ARB,
			custom::context_settings.flush_on_release ? WGL_CONTEXT_RELEASE_BEHAVIOR_FLUSH_ARB : WGL_CONTEXT_RELEASE_BEHAVIOR_NONE_ARB
		);
	}

	if (custom::context_settings.debug) {
		context_flags |= WGL_CONTEXT_DEBUG_BIT_ARB;
	}

	if (custom::context_settings.opengl_no_error && wgl.ARB_create_context_no_error) {
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
			CUSTOM_ASSERT(false, "WGL error '0x%x': failed to create context: invalid version", (u32)error);
		}
		else if (bits_are_set(error, ERROR_INVALID_PROFILE_ARB)) {
			CUSTOM_ASSERT(false, "WGL error '0x%x': failed to create context: invalid profile", (u32)error);
		}
		else if (bits_are_set(error, ERROR_INCOMPATIBLE_DEVICE_CONTEXTS_ARB)) {
			CUSTOM_ASSERT(false, "WGL error '0x%x': failed to create context: incopatible device context", (u32)error);
		}
		else {
			CUSTOM_ASSERT(false, "WGL error '0x%x': failed to create context: unknown", (u32)error);
		}
		return NULL;
	} LOG_LAST_ERROR();

	return hrc;
}
#undef SET_ATTRIBUTE

static HGLRC create_context_legacy(HDC hdc, HGLRC share_hrc) {
	// https://docs.microsoft.com/en-us/windows/win32/api/wingdi/nf-wingdi-wglcreatecontext
	// A rendering context is not the same as a device context. Set the pixel format of the device context before creating a rendering context. For more information on setting the device context's pixel format, see the SetPixelFormat function.
	// To use OpenGL, you create a rendering context, select it as a thread's current rendering context, and then call OpenGL functions. When you are finished with the rendering context, you dispose of it by calling the wglDeleteContext function.
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
		TEXT(CUSTOM_WINDOW_CLASS_NAME), TEXT(""),
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
		LOG_LAST_ERROR();
		CUSTOM_ASSERT(false, "failed to load " OPENGL_LIBRARY_NAME);
		return;
	}

	wgl.instance = opengl_handle;
	load_wgl_simple_functions();

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
	ReleaseDC(hwnd, hdc); LOG_LAST_ERROR();
	DestroyWindow(hwnd); LOG_LAST_ERROR();
}

static HGLRC platform_create_context(HDC hdc, HGLRC share_hrc) {
	if (custom::context_settings.api == custom::Context_Api::OpenGL) {
		if (!custom::context_settings.legacy_context && !wgl.ARB_create_context) {
			CUSTOM_ASSERT(false, "modern OpenGL context requires 'ARB_create_context'");
			return NULL;
		}

		if (!custom::context_settings.legacy_profile && !wgl.ARB_create_context_profile) {
			CUSTOM_ASSERT(false, "modern OpenGL profile requires 'ARB_create_context_profile'");
			return NULL;
		}
	}
	else if (custom::context_settings.api == custom::Context_Api::OpenGL_ES) {
		if (!wgl.EXT_create_context_es2_profile) {
			CUSTOM_ASSERT(false, "OpenGL ES requires 'EXT_create_context_es2_profile'");
			return NULL;
		}
	}

	custom::Pixel_Format pf = choose_pixel_format(hdc);
	wgl.pixel_format = pf;

	PIXELFORMATDESCRIPTOR pfd;
	if (!DescribePixelFormat(hdc, pf.id, sizeof(pfd), &pfd)) {
		LOG_LAST_ERROR();
		CUSTOM_ASSERT(false, "failed to describe pixel format %d", pf.id);
		return NULL;
	}
	wgl.pfd = pfd;

	if (!SetPixelFormat(hdc, pf.id, &pfd)) {
		LOG_LAST_ERROR();
		CUSTOM_ASSERT(false, "failed to set pixel format %d", pf.id);
		return NULL;
	}

	HGLRC hrc;
	if (wgl.ARB_create_context) {
		hrc = create_context_arb(hdc, share_hrc);
	}
	else {
		hrc = create_context_legacy(hdc, share_hrc);
	}

	// https://docs.microsoft.com/en-us/windows/win32/api/wingdi/nf-wingdi-wglmakecurrent
	// The hdc parameter must refer to a drawing surface supported by OpenGL. It need not be the same hdc that was passed to wglCreateContext when hglrc was created, but it must be on the same device and have the same pixel format. GDI transformation and clipping in hdc are not supported by the rendering context. The current rendering context uses the hdc device context until the rendering context is no longer current.
	// Before switching to the new rendering context, OpenGL flushes any previous rendering context that was current to the calling thread.
	// A thread can have one current rendering context. A process can have multiple rendering contexts by means of multithreading. A thread must set a current rendering context before calling any OpenGL functions. Otherwise, all OpenGL calls are ignored.
	// A rendering context can be current to only one thread at a time. You cannot make a rendering context current to multiple threads.
	// An application can perform multithread drawing by making different rendering contexts current to different threads, supplying each thread with its own rendering context and device context.
	// If an error occurs, the wglMakeCurrent function makes the thread's current rendering context not current before returning.
	if (hrc && !wgl.MakeCurrent(hdc, hrc)) {
		LOG_LAST_ERROR();
		wgl.MakeCurrent(NULL, NULL); LOG_LAST_ERROR();
		wgl.DeleteContext(hrc); LOG_LAST_ERROR();
		hrc = NULL;
		CUSTOM_ASSERT(false, "failed to make context current");
	} LOG_LAST_ERROR();

	return hrc;
}

static bool platform_swap_interval(HDC hdc, u32 value) {
	// https://www.khronos.org/registry/OpenGL/extensions/EXT/WGL_EXT_swap_control.txt
	if (wgl.EXT_swap_control) {
		return wgl.SwapIntervalEXT((s32)value);
	}
	return true;
}
