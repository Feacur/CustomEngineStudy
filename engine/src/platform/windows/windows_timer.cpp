#include "custom_pch.h"
#include "engine/debug/log.h"
#include "engine/platform/platform_timer.h"

#if !defined(CUSTOM_PRECOMPILED_HEADER)
	// #define WIN32_LEAN_AND_MEAN
	// #define NOMINMAX
	#include <Windows.h>
	#if WIN32_LEAN_AND_MEAN
		#include <timeapi.h> // winmm.lib
	#endif
#endif

// https://docs.microsoft.com/ru-ru/windows/win32/api/timeapi/nf-timeapi-timebeginperiod
// @Todo: scope each frame or scope entire runtime?
#define TIME_RESOLUTION 1
#define TIME_BEGIN() if (timeBeginPeriod(TIME_RESOLUTION) != TIMERR_NOERROR) {\
	CUSTOM_WARN("failed to adjust timer precision");\
}
#define TIME_END() if (timeEndPeriod(TIME_RESOLUTION) != TIMERR_NOERROR) {\
	CUSTOM_WARN("failed to adjust timer precision");\
}

//
// API implementation
//

LONGLONG platform_get_frequency();
LONGLONG platform_get_counter();

namespace custom
{
	Timer Timer::s_instance;

	Timer::Timer()
		: m_ticks_per_second(1000)
		, m_frame_start_ticks(0)
	{
		LONGLONG frequency = platform_get_frequency();
		m_ticks_per_second = (u64)frequency;
	}

	Timer::~Timer() = default;

	u64 Timer::snapshot()
	{
		u64 current_ticks = (u64)platform_get_counter();
		u64 frame_ticks = current_ticks - m_frame_start_ticks;
		m_frame_start_ticks = current_ticks;
		return frame_ticks;

	}

	u64 Timer::wait_next_frame(u64 duration, u64 precision)
	{
		u64 duration_ticks = mul_div(duration, m_ticks_per_second, precision);
		u64 frame_end_ticks = m_frame_start_ticks + duration_ticks;
		u64 current_ticks;
		TIME_BEGIN();
		while (true) {
			current_ticks = (u64)platform_get_counter();
			if (current_ticks >= frame_end_ticks) { break; }

			u64 sleep_milliseconds = mul_div(frame_end_ticks - current_ticks, 1000, m_ticks_per_second);
			Sleep((DWORD)sleep_milliseconds);
		}
		TIME_END();
		u64 frame_ticks = current_ticks - m_frame_start_ticks;
		m_frame_start_ticks = current_ticks;
		return frame_ticks;
	}
}

//
// platform implementation
//

LONGLONG platform_get_frequency() {
	LARGE_INTEGER value;
	QueryPerformanceFrequency(&value);
	return value.QuadPart;
}

LONGLONG platform_get_counter() {
	LARGE_INTEGER value;
	QueryPerformanceCounter(&value);
	return value.QuadPart;
}
