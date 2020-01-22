#pragma once
#include "engine/core/collection_types.h"

namespace custom {

struct Graphics_VM
{
	Graphics_VM();
	~Graphics_VM();

	void render(Array<u8> const & data);
};

}
