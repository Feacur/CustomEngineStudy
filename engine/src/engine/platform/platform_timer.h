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
		SOMEWHAT_A_SINGLETON_BOILERPLATE(Timer)

	public:
		~Timer();

		u64 wait_next_frame(u64 duration, u64 precision);

		u64 get_ticks_per_second() { return m_ticks_per_second; }
		u64 get_frame_start_ticks() { return m_frame_start_ticks; }

	private:
		u64 m_ticks_per_second;
		u64 m_frame_start_ticks;
	};
}
