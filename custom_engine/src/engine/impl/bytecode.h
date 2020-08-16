#pragma once
#include "engine/core/code.h"
#include "engine/debug/log.h"
#include "engine/api/internal/bytecode.h"
#include "engine/impl/array.h"

// https://en.cppreference.com/w/c/memory/malloc
// https://en.cppreference.com/w/c/language/object#Alignment

namespace custom {

// @Note: might come in handy in case of some portabiliy issues?
// #define alignof(type) ({ \
// 	struct s { char c; type d; }; \
// 	offsetof(struct s, d); \
// })

// @Note: it might be beneficial to align written data for future reads
//        - either because of performance of loading loading data
//        - or because of potential read failures and system failures
//        alternatively do not `read`, but `copy` data
//        *disclaimer*: didn't check actual benefits here, rather making
//        a proof of concept code for now
#define CUSTOM_GET_NEXT_MULTIPLE_2(value, multiple) ((value + (multiple - 1)) & ~(multiple - 1))
#define CUSTOM_GET_PADDING(T, value) (CUSTOM_GET_NEXT_MULTIPLE_2(value, (decltype(value))alignof(T)) - value)

template<typename T>
void Bytecode::write(T const * data, u32 count) {
	#if defined(CUSTOM_GET_PADDING)
	buffer.push_range(CUSTOM_GET_PADDING(T, buffer.count));
	#endif
	buffer.push_range((u8 *)data, count * sizeof(T));
}

template<typename T>
void Bytecode::write(T const & datum) {
	#if defined(CUSTOM_GET_PADDING)
	buffer.push_range(CUSTOM_GET_PADDING(T, buffer.count));
	#endif
	buffer.push_range((u8 *)&datum, sizeof(T));
}

template<typename T>
void Bytecode::write_sized_array(T const * data, u32 count) {
	write(count);
	write((T *)data, count);
}

template<typename T, u32 count>
void Bytecode::write_sized_array(T const (& data)[count]) {
	write(count);
	write((T *)data, count);
}

template<typename T>
void Bytecode::write_sized_array(Array<T> const & array) {
	write(array.count);
	write((T *)array.data, array.count);
}

template<typename T>
T const * Bytecode::read(u32 count) const {
	#if defined(CUSTOM_GET_PADDING)
	offset += CUSTOM_GET_PADDING(T, offset);
	#endif
	CUSTOM_ASSERT(offset + count * sizeof(T) <= buffer.count, "reading past written instructions");
	T * data = (T *)(buffer.data + offset);
	offset += count * sizeof(T);
	return data;
}

template<typename T>
void Bytecode::copy(T * out, u32 count) const {
	#if defined(CUSTOM_GET_PADDING)
	offset += CUSTOM_GET_PADDING(T, offset);
	#endif
	CUSTOM_ASSERT(offset + count * sizeof(T) <= buffer.count, "reading past written instructions");
	memcpy(out, buffer.data + offset, count * sizeof(T));
	offset += count * sizeof(T);
}

#undef CUSTOM_GET_NEXT_MULTIPLE_2
#undef CUSTOM_GET_PADDING

}
