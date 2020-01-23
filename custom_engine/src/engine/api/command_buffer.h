#pragma once
#include "engine/core/collection_types.h"

namespace custom {

struct Command_Buffer
{
	Array<u8> bytecode;
	u32 offset;

	template<typename T>
	void write(T const * data, u32 count);

	template<typename T>
	void write(T const & data);

	template<typename T>
	T const * read(u32 count = 1);
};

}
