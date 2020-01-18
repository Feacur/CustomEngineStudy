#pragma once

// https://docs.microsoft.com/en-us/windows/win32/api/wingdi/
typedef HGLRC (WINAPI CreateContext_func)(HDC hDc);
typedef BOOL  (WINAPI DeleteContext_func)(HGLRC oldContext);
typedef PROC  (WINAPI GetProcAddress_func)(LPCSTR lpszProc);
typedef BOOL  (WINAPI MakeCurrent_func)(HDC hDc, HGLRC newContext);
typedef BOOL  (WINAPI ShareLists_func)(HGLRC hrcSrvShare, HGLRC hrcSrvSource);

// https://www.khronos.org/registry/OpenGL/extensions/EXT
typedef cstring (WINAPI GetExtensionsStringEXT_func)(void);
typedef BOOL    (WINAPI SwapIntervalEXT_func)(void);
typedef int     (WINAPI GetSwapIntervalEXT_func)(void);

// https://www.khronos.org/registry/OpenGL/extensions/ARB/
typedef cstring (WINAPI GetExtensionsStringARB_func)(HDC hdc);
typedef HGLRC   (WINAPI CreateContextAttribsARB_func)(HDC hDC, HGLRC hShareContext, const int *attribList);
typedef BOOL    (WINAPI GetPixelFormatAttribivARB_func)(HDC hdc, int iPixelFormat, int iLayerPlane, UINT nAttributes, const int *piAttributes, int *piValues);
typedef BOOL    (WINAPI ChoosePixelFormatARB_func)(HDC hdc, const int *piAttribIList, const FLOAT *pfAttribFList, UINT nMaxFormats, int *piFormats, UINT *nNumFormats);

// attribute requests
#define WGL_NUMBER_PIXEL_FORMATS_ARB      0x2000

// attribute keys
#define WGL_DRAW_TO_WINDOW_ARB            0x2001
#define WGL_ACCELERATION_ARB              0x2003
#define WGL_SWAP_METHOD_ARB               0x2007
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

// attribute values
#define WGL_FULL_ACCELERATION_ARB         0x2027
#define WGL_SWAP_EXCHANGE_ARB             0x2028

// errors
#define ERROR_INVALID_VERSION_ARB         0x2095
#define ERROR_INVALID_PROFILE_ARB         0x2096
#define ERROR_INCOMPATIBLE_DEVICE_CONTEXTS_ARB 0x2054
