#pragma once
#include "engine/core/code.h"

namespace custom
{
	class System
	{
		SOMEWHAT_A_SINGLETON_BOILERPLATE(System)

	public:
		bool should_close;

	public:
		~System();

		void update();
		u64 get_system_time();
	};
}
