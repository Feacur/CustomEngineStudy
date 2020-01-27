#pragma once
#include "engine/core/collection_types.h"

namespace custom {

struct Bytecode
{
	Array<u8> buffer;
	mutable u32 offset;

	Bytecode();
	~Bytecode();

	template<typename T>
	void write(T const * data, u32 count);

	template<typename T>
	void write(T const & datum);

	template<typename T>
	T const * read(u32 count = 1) const;

	template<typename T>
	void copy(T * out, u32 count = 1) const;
};

}
