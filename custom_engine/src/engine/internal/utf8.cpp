#include "custom_pch.h"
#include "engine/core/utf8.h"
#include "engine/core/code.h"
#include "engine/debug/log.h"

// https://en.wikipedia.org/wiki/UTF-8
// http://www.unicode.org/versions/Unicode6.2.0/UnicodeStandard-6.2.pdf

// encoding template (from a N-bytes array)
// - to decode, mask and tightly pack all the 'x'
// 4 bytes: 0b 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
// 3 bytes: 0b 1110xxxx 10xxxxxx 10xxxxxx ________
// 2 bytes: 0b 110xxxxx 10xxxxxx ________ ________
// 1 byte:  0b 0xxxxxxx ________ ________ ________

// decoding template (from a 4-byte value)
// 4 bytes: 0b 00000000 000xxxxx xxxxxxxx xxxxxxxx ( range is 0x 00010000-0010ffff )
// 3 bytes: 0b 00000000 00000000 xxxxxxxx xxxxxxxx ( range is 0x 00000800-0000ffff )
// 2 bytes: 0b 00000000 00000000 00000xxx xxxxxxxx ( range is 0x 00000080-000007ff )
// 1 byte:  0b 00000000 00000000 00000000 0xxxxxxx ( range is 0x 00000000-0000007f )

namespace custom {
namespace unicode {

struct decoding {
	u8 signature, mask, leading, continuation;
};

constexpr static decoding const table[] = {
	// signature  mask        lead. byte  cont. byte
	{ 0b11110000, 0b11111000, 0b00000111, 0b00111111, }, // 21 bits = 3 + 6 + 6 + 6
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
	// @Note: might drop this security check
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
	// @Note: might drop this security check
	if ((buffer[0] & table[3].mask) == table[3].signature) {
		return buffer + 1;
	}
	CUSTOM_ASSERT(false, "trying to decode a single continuation byte");
	return NULL;
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
