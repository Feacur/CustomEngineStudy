#pragma once
#include "engine/api/command_buffer.h"
#include "engine/core/code.h"
#include "engine/debug/log.h"

namespace custom {

template<typename T>
void Command_Buffer::write(T const * data, u32 count) {
	bytecode.push_range((u8 *)data, count * sizeof(T));
}

template<typename T>
void Command_Buffer::write(T const & datum) {
	bytecode.push_range((u8 *)&datum, sizeof(T));
}

template<typename T>
T const * Command_Buffer::read(u32 count) {
	CUSTOM_ASSERT(offset + count * sizeof(T) <= bytecode.count, "reading past written instructions");
	T * data = (T *)(bytecode.data + offset);
	offset += count * sizeof(T);
	return data;
}

}
