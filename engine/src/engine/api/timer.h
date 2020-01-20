#pragma once
#include "engine/core/code.h"

namespace custom
{
	// @Note: a singleton? a static instance? a static class/struct? a namespace?
	struct Timer {
		u64 ticks_per_second;
		u64 frame_start_ticks;
	};
	extern Timer timer;

	void timer_init();
	u64  timer_snapshot();
	u64  timer_wait_next_frame(u64 duration, u64 precision);

	extern u64 millisecond;
	extern u64 microsecond;
	extern u64 nanosecond;
}
