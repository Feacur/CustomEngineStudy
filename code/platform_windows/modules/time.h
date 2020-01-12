// #include <timeapi.h> // (requires winmm.lib) timeBeginPeriod, timeEndPeriod

// static TIMECAPS time_caps;
static LARGE_INTEGER ticks_per_second;
static LARGE_INTEGER clock_current;
static LARGE_INTEGER clock_game_start;
static LARGE_INTEGER clock_frame_start;

void initialize_time() {
	// if (timeGetDevCaps(&time_caps, sizeof(time_caps)) != MMSYSERR_NOERROR) {
	// 	CUSTOM_ERROR("Can't retrieve time caps");
	// }

	QueryPerformanceFrequency(&ticks_per_second);
	QueryPerformanceCounter(&clock_game_start);
	clock_game_start = clock_frame_start = clock_game_start;
}

int64 get_clock_span_get_clock_spanseconds(LARGE_INTEGER start, LARGE_INTEGER end, int64 precision) {
	return mul_div(end.QuadPart - start.QuadPart, precision, ticks_per_second.QuadPart);
}

int64 wait_for_next_frame(int64 target_delta, int64 precision) {
	static int64 const target_precision = 1000;
	
	// if (time_caps.wPeriodMin != 0) {
	// 	if (timeBeginPeriod(time_caps.wPeriodMin) != TIMERR_NOERROR) {
	// 		CUSTOM_ERROR("Timer granularity wasn't set");
	// 	}
	// }

	QueryPerformanceCounter(&clock_current);
	int64 delta = get_clock_span_get_clock_spanseconds(clock_frame_start, clock_current, precision);
	
	if (delta < target_delta) {
		DWORD sleep_millis = (DWORD)mul_div(target_delta - delta, target_precision, precision);
		Sleep(sleep_millis);
		do {
			QueryPerformanceCounter(&clock_current);
			delta = get_clock_span_get_clock_spanseconds(clock_frame_start, clock_current, precision);
		} while (delta < target_delta);
	}
	
	// if (time_caps.wPeriodMin != 0) {
	// 	if (timeEndPeriod(time_caps.wPeriodMin) != TIMERR_NOERROR) {
	// 		CUSTOM_ERROR("Timer granularity wasn't reset");
	// 	}
	// }

	clock_frame_start = clock_current;
	return delta;
}

uint64 get_timestamp() {
	FILETIME time;
	GetSystemTimeAsFileTime(&time);

	ULARGE_INTEGER uintTime;
	uintTime.LowPart = time.dwLowDateTime;
	uintTime.HighPart = time.dwHighDateTime;

	return uintTime.QuadPart;
}
