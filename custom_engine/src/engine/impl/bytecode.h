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

template<typename T>
void Bytecode::write(T const * data, u32 count) {
	write_bytes(alignof(T), (u8 const *)data, count * sizeof(T));
}

template<typename T>
T const * Bytecode::read(u32 count) const {
	return (T const *)read_bytes(alignof(T), count * sizeof(T));
}

template<typename T>
void Bytecode::copy(T * out, u32 count) const {
	copy_bytes(alignof(T), (u8 *)out, count * sizeof(T));
}

}
