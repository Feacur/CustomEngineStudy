#pragma once
#include "engine/core/code.h"

namespace custom
{
	constexpr inline u64 mul_div(u64 value, u64 numerator, u64 denominator) {
		u64 a = value / denominator;
		u64 b = value % denominator;
		return a * numerator + b * numerator / denominator;
	}

	class Timer
	{
	public:
		Timer();
		~Timer();

		u64 wait_next_frame(u64 duration, u64 precision);

		u64 get_ticks_per_second() { return ticks_per_second; }
		u64 get_frame_start_ticks() { return frame_start_ticks; }

	private:
		u64 ticks_per_second;
		u64 frame_start_ticks;
	};
}
extern custom::Timer global_timer;
