#include "custom_pch.h"
#include "engine/core/utf8.h"
#include "engine/core/code.h"
#include "engine/debug/log.h"

// https://en.wikipedia.org/wiki/UTF-8

namespace custom {
namespace unicode {

struct encoding {
	u8 signature, mask, leading, continuation;
};

static encoding const table[] = {
	// signature  mask        lead. byte  cont. byte
	{ 0b11110000, 0b11111000, 0b00000111, 0b00111111, }, // 21 bit  = 3 + 6 + 6 + 6
	{ 0b11100000, 0b11110000, 0b00001111, 0b00111111, }, // 16 bits = 4 + 6 + 6 __0
	{ 0b11000000, 0b11100000, 0b00011111, 0b00111111, }, // 11 bits = 5 + 6 __0 __0
	{ 0b00000000, 0b10000000, 0b01111111, 0b00000000, }, //  7 bits = 7 __0 __0 __0
};

u32 decode(utf8 buffer) {
	if ((buffer[0] & table[0].mask) == table[0].signature) { // 4 bytes
		return (buffer[0] & table[0].leading)      << 18
		     | (buffer[1] & table[0].continuation) << 12
		     | (buffer[2] & table[0].continuation) <<  6
		     | (buffer[3] & table[0].continuation);
	}
	if ((buffer[0] & table[1].mask) == table[1].signature) { // 3 bytes
		return (buffer[0] & table[1].leading)      << 12
		     | (buffer[1] & table[1].continuation) <<  6
		     | (buffer[2] & table[1].continuation);
	}
	if ((buffer[0] & table[2].mask) == table[2].signature) { // 2 bytes
		return (buffer[0] & table[2].leading)      <<  6
		     | (buffer[2] & table[2].continuation);
	}
	if ((buffer[0] & table[3].mask) == table[3].signature) { // 1 byte
	 	return (buffer[0] & table[3].leading);
	}
	CUSTOM_ASSERT(false, "trying to decode a single continuation byte");
	return 0;
}

utf8 next(utf8 buffer) {
	if ((buffer[0] & table[0].mask) == table[0].signature) {
		return buffer + 4;
	}
	if ((buffer[0] & table[1].mask) == table[1].signature) {
		return buffer + 3;
	}
	if ((buffer[0] & table[2].mask) == table[2].signature) {
		return buffer + 2;
	}
	if ((buffer[0] & table[3].mask) == table[3].signature) {
		return buffer + 1;
	}
	return buffer;
}

u32 count(utf8 buffer) {
	u32 count = 0;
	while (*buffer) {
		++count;
		buffer = next(buffer);
	}
	return count;
}

}}
