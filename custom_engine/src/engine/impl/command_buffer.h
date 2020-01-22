#pragma once
#include "engine/api/command_buffer.h"
#include "engine/core/code.h"
#include "engine/debug/log.h"

namespace custom {

template<typename T>
void Command_Buffer::write(T const * data, u32 count) {
	u32 bytes_to_write = count * sizeof(T);
	bytecode.ensure_capacity(bytecode.count + bytes_to_write);
	memcpy(bytecode.data + bytecode.count, data, bytes_to_write);
	bytecode.count += bytes_to_write;
}

template<typename T>
void Command_Buffer::write(T const & data) {
	write(&data, 1);
}

template<typename T>
T const * Command_Buffer::read(u32 count) {
	CUSTOM_ASSERT(offset + count * sizeof(T) <= bytecode.count, "reading past written instructions");
	T * data = (T *)(bytecode.data + offset);
	offset += count * sizeof(T);
	return data;
}

}
