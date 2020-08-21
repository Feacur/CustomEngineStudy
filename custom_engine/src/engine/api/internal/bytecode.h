#pragma once
#include "engine/core/collection_types.h"

namespace custom {

struct Bytecode
{
	// @Todo: figure out alignment issues; concerning `realloc` especially
	//        - replace with Array<u64> or Array<size_t>?
	//        - actually verify and profile benefits
	Array<u8> buffer;
	mutable u32 read_offset;

	Bytecode(void);
	~Bytecode(void) = default;

	void reset(void);

	void write_bytes(u8 alignment, u8 const * data, u32 count);
	u8 const * read_bytes(u8 alignment, u32 count) const;
	void copy_bytes(u8 alignment, u8 * out, u32 count) const;

	template<typename T> void      write(T const * data, u32 count);
	template<typename T> T const * read(u32 count = 1) const;
	template<typename T> void      copy(T * out, u32 count = 1) const;

	template<typename T> inline void write(T const & datum) { write(&datum, 1); }
	
};

}
