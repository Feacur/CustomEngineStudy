#pragma once
#include "engine/api/internal/bytecode.h"

// https://en.cppreference.com/w/c/memory/malloc
// https://en.cppreference.com/w/c/language/object#Alignment

namespace custom {

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
