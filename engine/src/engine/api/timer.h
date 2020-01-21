#pragma once
#include "engine/core/code.h"

namespace custom
{
	// @Note: [ ] a singleton?           - is error-prone due to copy-construction
	//        [ ] a static class/struct? - is merely a namespace in the end
	//        [ ] a namespace?           - is nice to scope some data
	//        [x] a static instance?     - is a scoped data holder
	struct Timer {
		u64 ticks_per_second;
		u64 frame_start_ticks;
	};

	// @Note: a static instance; Timer::instance would be the same data-wise
	extern Timer timer;

	void timer_init();
	u64  timer_snapshot();
	u64  timer_wait_next_frame(u64 duration, u64 precision);

	static constexpr u64 const millisecond = 1000;
	static constexpr u64 const microsecond = 1000000;
	static constexpr u64 const nanosecond  = 1000000000;
}
