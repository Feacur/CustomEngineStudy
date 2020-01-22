#pragma once
#include "engine/core/collection_types.h"

namespace custom {

struct Rendering_VM
{
	Rendering_VM();
	~Rendering_VM();

	void update();

private:
	Array<u8> data;
};

}
