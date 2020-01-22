#include "custom_pch.h"
#include "engine/core/utf8.h"
#include "engine/core/code.h"
#include "engine/debug/log.h"

// https://en.wikipedia.org/wiki/UTF-8

namespace custom {
namespace unicode {

struct encoding {
	u8 signature, mask, lead, continuation;
};

static encoding const table[] = {
//    signature   mask        lead. byte  cont. byte
	{ 0b11110000, 0b11111000, 0b00000111, 0b00111111, }, // 21 bit  = 3 + 6 + 6 + 6
	{ 0b11100000, 0b11110000, 0b00001111, 0b00111111, }, // 16 bits = 4 + 6 + 6 __0
	{ 0b11000000, 0b11100000, 0b00011111, 0b00111111, }, // 11 bits = 5 + 6 __0 __0
	{ 0b00000000, 0b10000000, 0b01111111, 0b00000000, }, //  7 bits = 7 __0 __0 __0
};

u32 decode(utf8 value) {
	if ((value[0] & table[0].mask) == table[0].signature) { // 4 bytes
		return (value[0] & table[0].lead)         << 18
		     | (value[1] & table[0].continuation) << 12
		     | (value[2] & table[0].continuation) <<  6
		     | (value[3] & table[0].continuation);
	}
	if ((value[0] & table[1].mask) == table[1].signature) { // 3 bytes
		return (value[0] & table[1].lead)         << 12
		     | (value[1] & table[1].continuation) <<  6
		     | (value[2] & table[1].continuation);
	}
	if ((value[0] & table[2].mask) == table[2].signature) { // 2 bytes
		return (value[0] & table[2].lead)         <<  6
		     | (value[2] & table[2].continuation);
	}
	if ((value[0] & table[3].mask) == table[3].signature) { // 1 byte
	 	return (value[0] & table[3].lead);
	}
	CUSTOM_ASSERT(false, "trying to decode a single continuation byte");
	return 0;
}

utf8 next(utf8 value) {
	if ((value[0] & table[0].mask) == table[0].signature) {
		return value + 4;
	}
	if ((value[0] & table[1].mask) == table[1].signature) {
		return value + 3;
	}
	if ((value[0] & table[2].mask) == table[2].signature) {
		return value + 2;
	}
	if ((value[0] & table[3].mask) == table[3].signature) {
		return value + 1;
	}
	return value;
}

u32 count(utf8 value) {
	u32 count = 0;
	while (*value) {
		++count;
		value = next(value);
	}
	return count;
}

}}
