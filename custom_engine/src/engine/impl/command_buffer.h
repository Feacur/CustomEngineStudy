#pragma once
#include "engine/api/command_buffer.h"
#include "engine/impl/array.h"
#include "engine/core/code.h"
#include "engine/debug/log.h"

namespace custom {

// @Note: might come in handy in case of some portabiliy issues?
// #define alignof(type) ({ \
// 	struct s { char c; type d; }; \
// 	offsetof(struct s, d); \
// })

// @Note: it might be beneficial to align written data for future reads
//        - either because of performance of loading loading data
//        - or because of potential read failures and system failures
//        alternatively do not 'read', but 'copy' data
//        *disclaimer*: didn't check actual benefits here; rather making
//        a proof of concept code for now
#define CUSTOM_GET_PADDING(T, value) (((value + alignof(T) - 1u) / alignof(T)) * alignof(T) - value)
// #if !defined(CUSTOM_GET_PADDING)
// 	#define CUSTOM_GET_PADDING(T, value) 0
// #endif

template<typename T>
void Command_Buffer::write(T const * data, u32 count) {
	#if defined(CUSTOM_GET_PADDING)
	bytecode.push_range(CUSTOM_GET_PADDING(T, bytecode.count));
	#endif
	bytecode.push_range((u8 *)data, count * sizeof(T));
}

template<typename T>
void Command_Buffer::write(T const & datum) {
	#if defined(CUSTOM_GET_PADDING)
	bytecode.push_range(CUSTOM_GET_PADDING(T, bytecode.count));
	#endif
	bytecode.push_range((u8 *)&datum, sizeof(T));
}

template<typename T>
T const * Command_Buffer::read(u32 count) const {
	#if defined(CUSTOM_GET_PADDING)
	offset += CUSTOM_GET_PADDING(T, offset);
	#endif
	CUSTOM_ASSERT(offset + count * sizeof(T) <= bytecode.count, "reading past written instructions");
	T * data = (T *)(bytecode.data + offset);
	offset += count * sizeof(T);
	return data;
}

template<typename T>
void Command_Buffer::copy(T * out, u32 count) const {
	#if defined(CUSTOM_GET_PADDING)
	offset += CUSTOM_GET_PADDING(T, offset);
	#endif
	CUSTOM_ASSERT(offset + count * sizeof(T) <= bytecode.count, "reading past written instructions");
	memcpy(out, bytecode.data + offset, count * sizeof(T));
	offset += count * sizeof(T);
}

#undef CUSTOM_GET_PADDING

}
