#include "custom_pch.h"

#include "engine/impl/bytecode.h"

// @Note: it might be beneficial to align written data for future reads
//        - either because of performance of loading loading data
//        - or because of potential read failures and system failures
//        alternatively do not `read`, but `copy` data
//        *disclaimer*: didn't check actual benefits here, rather making
//        a proof of concept code for now
#define CUSTOM_GET_NEXT_MULTIPLE_2(value, multiple) ((value + (multiple - 1)) & ~(multiple - 1))
#define CUSTOM_ALIGN(value, alignment) value = CUSTOM_GET_NEXT_MULTIPLE_2(value, (decltype(value))alignment)
// #define CUSTOM_ALIGN(value, alignment) (void)0

namespace custom {

Bytecode::Bytecode(void)
	: buffer()
	, read_offset(0)
{ }

void Bytecode::reset(void) {
	buffer.count = 0;
	read_offset = 0;
}

void Bytecode::write_bytes(u8 alignment, u8 const * data, u32 count) {
	CUSTOM_ALIGN(buffer.count, alignment);
	buffer.push_range(data, count);
}

u8 const * Bytecode::read_bytes(u8 alignment, u32 count) const {
	CUSTOM_ALIGN(read_offset, alignment);
	if (read_offset + count > buffer.count) {
		CUSTOM_ASSERT(false, "reading past written instructions");
		return NULL;
	}
	u8 const * data = buffer.data + read_offset;
	read_offset += count;
	return data;
}

void Bytecode::copy_bytes(u8 alignment, u8 * out, u32 count) const {
	CUSTOM_ALIGN(read_offset, alignment);
	if (read_offset + count > buffer.count) {
		CUSTOM_ASSERT(false, "reading past written instructions");
		return;
	}
	memcpy(out, buffer.data + read_offset, count);
	read_offset += count;
}

}
