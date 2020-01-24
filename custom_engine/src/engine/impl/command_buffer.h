#pragma once
#include "engine/api/command_buffer.h"
#include "engine/core/code.h"
#include "engine/debug/log.h"

namespace custom {

// @Note: it might be beneficial to align written data for future reads
//        - either because of performance of loading loading data
//        - or because of potential read failures and system failures
//        alternatively do not 'read', but 'copy' data
//        *disclaimer*: didn't check actual benefits here; rather making
//        a proof of concept code for now
#define CUSTOM_GET_PADDING(T, value) (((value + sizeof(T) - 1u) / sizeof(T)) * sizeof(T) - value)
#if !defined(CUSTOM_GET_PADDING)
	#define CUSTOM_GET_PADDING(T, value) 0
#endif

template<typename T>
void Command_Buffer::write(T const * data, u32 count) {
	bytecode.push_range(CUSTOM_GET_PADDING(T, bytecode.count));
	bytecode.push_range((u8 *)data, count * sizeof(T));
}

template<typename T>
void Command_Buffer::write(T const & datum) {
	bytecode.push_range(CUSTOM_GET_PADDING(T, bytecode.count));
	bytecode.push_range((u8 *)&datum, sizeof(T));
}

template<typename T>
T const * Command_Buffer::read(u32 count) const {
	offset += CUSTOM_GET_PADDING(T, offset);
	CUSTOM_ASSERT(offset + count * sizeof(T) <= bytecode.count, "reading past written instructions");
	T * data = (T *)(bytecode.data + offset);
	offset += count * sizeof(T);
	return data;
}

template<typename T>
void Command_Buffer::copy(T * out, u32 count) const {
	offset += CUSTOM_GET_PADDING(T, offset);
	CUSTOM_ASSERT(offset + count * sizeof(T) <= bytecode.count, "reading past written instructions");
	memcpy(out, bytecode.data + offset, count * sizeof(T));
	offset += count * sizeof(T);
}

#undef CUSTOM_GET_PADDING

}
