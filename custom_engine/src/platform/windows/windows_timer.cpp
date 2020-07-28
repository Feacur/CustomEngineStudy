#include "custom_pch.h"
#include "engine/api/platform/timer.h"
#include "engine/core/code.h"
#include "engine/debug/log.h"

#if !defined(CUSTOM_PRECOMPILED_HEADER)
	#include <Windows.h>
	#if WIN32_LEAN_AND_MEAN
		#include <timeapi.h>
	#endif
#endif

// #define TIMER_ADJUST_PRECISION

// https://docs.microsoft.com/en-us/windows/win32/api/synchapi/nf-synchapi-sleep
// https://docs.microsoft.com/ru-ru/windows/win32/api/timeapi/nf-timeapi-timebeginperiod
// @Todo: scope each frame or scope entire runtime?
static UINT system_timer_period;
#if defined(TIMER_ADJUST_PRECISION)
	#define TIME_BEGIN() if (timeBeginPeriod(system_timer_period) != TIMERR_NOERROR) {\
		CUSTOM_WARNING("failed to adjust timer precision");\
	}\

	#define TIME_END() if (timeEndPeriod(system_timer_period) != TIMERR_NOERROR) {\
		CUSTOM_WARNING("failed to adjust timer precision");\
	}\

#else
	#define TIME_BEGIN() (void)0
	#define TIME_END() (void)0
#endif

#if !defined(CUSTOM_SHIPPING)
	extern void log_last_error(cstring source);
	#define LOG_LAST_ERROR() log_last_error(CUSTOM_FILE_AND_LINE)
#else
	#define LOG_LAST_ERROR() (void)0
#endif

// #include "engine/math/scalar.h"
constexpr static inline u64 mul_div(u64 value, u64 numerator, u64 denominator) {
	u64 a = value / denominator;
	u64 b = value % denominator;
	return a * numerator + b * numerator / denominator;
}

//
// API implementation
//

static UINT     platform_get_resolution(void);
static LONGLONG platform_get_frequency(void);
static LONGLONG platform_get_counter(void);

namespace custom {
namespace timer {

void init(void) {
	ticks_per_second = (u64)platform_get_frequency();
	system_timer_period = platform_get_resolution();
	snapshot();
}

u64 get_ticks(void) {
	return (u64)platform_get_counter();
}

u64 snapshot(void) {
	u64 current_ticks = (u64)platform_get_counter();
	u64 frame_ticks = current_ticks - frame_start_ticks;
	frame_start_ticks = current_ticks;
	return frame_ticks;
}

u64 wait_next_frame(u64 duration, u64 precision)
{
	u64 duration_ticks = mul_div(duration, ticks_per_second, precision);
	u64 frame_end_ticks = frame_start_ticks + duration_ticks;
	u64 current_ticks;
	TIME_BEGIN();
	while (true) {
		current_ticks = (u64)platform_get_counter();
		if (current_ticks >= frame_end_ticks) { break; }

		u64 sleep_milliseconds = mul_div(frame_end_ticks - current_ticks, millisecond, ticks_per_second);
		Sleep((DWORD)sleep_milliseconds);
	}
	TIME_END();
	u64 frame_ticks = current_ticks - frame_start_ticks;
	frame_start_ticks = current_ticks;
	return frame_ticks;
}

}}

//
// platform implementation
//

static UINT platform_get_resolution(void) {
	TIMECAPS timecaps;
	MMRESULT status = timeGetDevCaps(&timecaps, sizeof(timecaps));
	if (status != MMSYSERR_NOERROR) {
		LOG_LAST_ERROR();
		CUSTOM_WARNING("failed to retrieve timer resolution");
		return 1;
	}
	return timecaps.wPeriodMin;
}

static LONGLONG platform_get_frequency(void) {
	LARGE_INTEGER value;
	BOOL status = QueryPerformanceFrequency(&value);
	if (!status) {
		LOG_LAST_ERROR();
		CUSTOM_ASSERT(false, "failed to retieve performance frequency");
		return 1000;
	}
	return value.QuadPart;
}

static LONGLONG platform_get_counter(void) {
	LARGE_INTEGER value;
	QueryPerformanceCounter(&value);
	return value.QuadPart;
}
