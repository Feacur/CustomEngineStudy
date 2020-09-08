#pragma once

#if defined(CUSTOM_PRECOMPILED_HEADER)

// core
#include <signal.h>
#include <stdint.h>
#include <float.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include <new>

#if defined(_MSC_VER)
	#include <intrin.h>
#elif defined(__GNUC__) || defined(__clang__)
	#include <x86intrin.h>
#else
	#pragma message("no intrinsics header included")
#endif

// platform
#if defined(_WIN64) || defined(_WIN32)
	#include <Windows.h>
	#if WIN32_LEAN_AND_MEAN
		#include <timeapi.h>
	#endif
#endif

// vendor
#include <glad/glad.h>
#include <stb_image.h>
#include <lua.hpp>

#endif
