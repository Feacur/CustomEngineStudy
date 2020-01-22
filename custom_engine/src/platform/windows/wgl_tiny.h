#pragma once

// https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_debug_output.txt
// Debug contexts
// A context can be created in debug mode. This allows, among other things, debug output functionality to work more effectively.

// https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_create_context_no_error.txt
// No error contexts
// An OpenGL Context can be created which does not report OpenGL Errors. If the context bit GL_CONTEXT_FLAG_NO_ERROR_BIT is set to true, then the context will not report most errors. It will still report GL_OUT_OF_MEMORY_ERROR where appropriate, but this can be delayed from the point where the error actually happens. No other errors will be reported.
// This also means that the implementation will not check for errors either. So if you provide incorrect parameters to a function that would have provoked an error, you will get undefined behavior instead. This includes the possibility of application termination.
// Contexts cannot have the no error bit and the robustsness or debug bits.

// https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_robustness.txt
// Robust access context
// Normally, many buffer memory access operations that access data outside of the bound range of storage have undefined results (potentially including program termination). However, if the program doesn't crash, this can lead to being able to read values that were written by other applications, which is a security concern.
// Robust access means that out-of-bounds reads will provide well-defined results (usually zero). And such accesses will never cause program termination. It provides better process isolation.

// https://docs.microsoft.com/en-us/windows/win32/api/wingdi/
typedef HGLRC (WINAPI CreateContext_func)(HDC hDc);
typedef BOOL  (WINAPI DeleteContext_func)(HGLRC oldContext);
typedef PROC  (WINAPI GetProcAddress_func)(LPCSTR lpszProc);
typedef BOOL  (WINAPI MakeCurrent_func)(HDC hDc, HGLRC newContext);
typedef BOOL  (WINAPI ShareLists_func)(HGLRC hrcSrvShare, HGLRC hrcSrvSource);
// typedef HDC   (WINAPI GetCurrentDC_func)(void);
// typedef HGLRC (WINAPI GetCurrentContext_func)(void);

// https://www.khronos.org/registry/OpenGL/extensions/EXT
typedef cstring (WINAPI GetExtensionsStringEXT_func)(void);
typedef BOOL    (WINAPI SwapIntervalEXT_func)(int interval);
typedef int     (WINAPI GetSwapIntervalEXT_func)(void);

// https://www.khronos.org/registry/OpenGL/extensions/ARB/
typedef cstring (WINAPI GetExtensionsStringARB_func)(HDC hdc);
typedef HGLRC   (WINAPI CreateContextAttribsARB_func)(HDC hDC, HGLRC hShareContext, const int *attribList);
typedef BOOL    (WINAPI GetPixelFormatAttribivARB_func)(HDC hdc, int iPixelFormat, int iLayerPlane, UINT nAttributes, const int *piAttributes, int *piValues);
// typedef BOOL    (WINAPI ChoosePixelFormatARB_func)(HDC hdc, const int *piAttribIList, const FLOAT *pfAttribFList, UINT nMaxFormats, int *piFormats, UINT *nNumFormats);

// pixel format attribute keys
#define WGL_NUMBER_PIXEL_FORMATS_ARB      0x2000
#define WGL_DRAW_TO_WINDOW_ARB            0x2001
#define WGL_ACCELERATION_ARB              0x2003
#define WGL_SWAP_METHOD_ARB               0x2007
#define WGL_TRANSPARENT_ARB               0x200A
#define WGL_SUPPORT_OPENGL_ARB            0x2010
#define WGL_DOUBLE_BUFFER_ARB             0x2011
#define WGL_STEREO_ARB                    0x2012
#define WGL_PIXEL_TYPE_ARB                0x2013
#define WGL_RED_BITS_ARB                  0x2015
#define WGL_RED_SHIFT_ARB                 0x2016
#define WGL_GREEN_BITS_ARB                0x2017
#define WGL_GREEN_SHIFT_ARB               0x2018
#define WGL_BLUE_BITS_ARB                 0x2019
#define WGL_BLUE_SHIFT_ARB                0x201A
#define WGL_ALPHA_BITS_ARB                0x201B
#define WGL_ALPHA_SHIFT_ARB               0x201C
#define WGL_ACCUM_BITS_ARB                0x201D
#define WGL_ACCUM_RED_BITS_ARB            0x201E
#define WGL_ACCUM_GREEN_BITS_ARB          0x201F
#define WGL_COLOR_BITS_ARB                0x2014
#define WGL_ACCUM_BLUE_BITS_ARB           0x2020
#define WGL_ACCUM_ALPHA_BITS_ARB          0x2021
#define WGL_DEPTH_BITS_ARB                0x2022
#define WGL_STENCIL_BITS_ARB              0x2023
#define WGL_AUX_BUFFERS_ARB               0x2024
#define WGL_SAMPLES_ARB                   0x2042
#define WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB  0x20A9
#define WGL_COLORSPACE_EXT                0x309D

// pixel format attribute values
#define WGL_NO_ACCELERATION_ARB           0x2025
#define WGL_GENERIC_ACCELERATION_ARB      0x2026
#define WGL_FULL_ACCELERATION_ARB         0x2027
#define WGL_SWAP_EXCHANGE_ARB             0x2028
#define WGL_SWAP_COPY_ARB                 0x2029
#define WGL_SWAP_UNDEFINED_ARB            0x202A
#define WGL_TYPE_RGBA_ARB                 0x202B
#define WGL_TYPE_COLORINDEX_ARB           0x202C
#define WGL_COLORSPACE_SRGB_EXT           0x3089

// errors
#define ERROR_INVALID_VERSION_ARB         0x2095
#define ERROR_INVALID_PROFILE_ARB         0x2096
#define ERROR_INCOMPATIBLE_DEVICE_CONTEXTS_ARB 0x2054

// context creation keys
#define WGL_CONTEXT_MAJOR_VERSION_ARB     0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB     0x2092
#define WGL_CONTEXT_RELEASE_BEHAVIOR_ARB  0x2097
#define WGL_CONTEXT_OPENGL_NO_ERROR_ARB   0x31B3
#define WGL_CONTEXT_RESET_NOTIFICATION_STRATEGY_ARB 0x8256

// context creation values
#define WGL_CONTEXT_RELEASE_BEHAVIOR_NONE_ARB 0
#define WGL_CONTEXT_RELEASE_BEHAVIOR_FLUSH_ARB 0x2098
#define WGL_LOSE_CONTEXT_ON_RESET_ARB     0x8252
#define WGL_NO_RESET_NOTIFICATION_ARB     0x8261

// context creation flag key and bits
#define WGL_CONTEXT_FLAGS_ARB             0x2094
#define WGL_CONTEXT_DEBUG_BIT_ARB         0x00000001
#define WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB 0x00000002
#define WGL_CONTEXT_ROBUST_ACCESS_BIT_ARB 0x00000004

// context creation mask key and bits
#define WGL_CONTEXT_PROFILE_MASK_ARB      0x9126
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB  0x00000001
#define WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB 0x00000002
#define WGL_CONTEXT_ES2_PROFILE_BIT_EXT   0x00000004
