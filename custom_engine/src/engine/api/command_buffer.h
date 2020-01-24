#pragma once
#include "engine/core/collection_types.h"

namespace custom {

struct Command_Buffer
{
	Array<u8> bytecode;
	mutable u32 offset; // @Todo: make it an explicit parameter of read(...)?

	template<typename T>
	void write(T const * data, u32 count);

	template<typename T>
	void write(T const & datum);

	template<typename T>
	T const * read(u32 count = 1) const;
};

}
