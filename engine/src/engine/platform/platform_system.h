#pragma once
#include "engine/core/code.h"

namespace custom
{
	class System
	{
	public:
		System();
		~System();

		void update();
		u64 get_system_time();

		bool is_running;
	};
}
extern custom::System global_system;
