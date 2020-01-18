#pragma once

#if defined(CUSTOM_PRECOMPILED_HEADER)

#include <signal.h>

#if defined(_WIN64) || defined(_WIN32)
	#include <Windows.h>
	#if WIN32_LEAN_AND_MEAN
		#include <timeapi.h>
	#endif
#endif

#endif
