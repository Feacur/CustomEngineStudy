#include "custom_pch.h"
#include "engine/platform/platform_timer.h"
#include "engine/debug/log.h"

#if !defined(CUSTOM_PRECOMPILED_HEADER)
	#include <Windows.h>
#endif

custom::Timer global_timer;

LONGLONG platform_get_frequency();
LONGLONG platform_get_counter();

//
// API implementation
//

namespace custom
{
	Timer::Timer()
	{
		LONGLONG frequency = platform_get_frequency();
		ticks_per_second = (u64)frequency;
	}

	Timer::~Timer() = default;

	u64 Timer::wait_next_frame(u64 duration, u64 precision)
	{
		u64 duration_ticks = mul_div(duration, ticks_per_second, precision);
		u64 frame_end_ticks = frame_start_ticks + duration_ticks;
		u64 current_ticks;
		while(true) {
			current_ticks = (u64)platform_get_counter();
			if (current_ticks >= frame_end_ticks) { break; }

			u64 sleep_milliseconds = mul_div(frame_end_ticks - current_ticks, 1000, ticks_per_second);
			Sleep((DWORD)sleep_milliseconds);
		}
		u64 frame_ticks = current_ticks - frame_start_ticks;
		frame_start_ticks = current_ticks;
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

// https://docs.microsoft.com/ru-ru/windows/win32/api/timeapi/nf-timeapi-timebeginperiod
// TIMERR_NOERROR, TIMERR_NOCANDO
// timeBeginPeriod(1);
// timeEndPeriod(1);
