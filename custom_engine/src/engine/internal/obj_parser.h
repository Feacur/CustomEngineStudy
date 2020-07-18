// #include "engine/api/platform/timer.h"

// https://github.com/syoyo/tinyobjloader/blob/master/tiny_obj_loader.h
// https://github.com/rlk/obj/blob/master/obj.c
// https://github.com/Oblomov/faster-parser/blob/master/parse-float.h

#include <math.h>

namespace custom {
namespace obj {

inline static bool IS_BLANK(char value) { return (value == ' ')  || (value == '\t'); }
inline static bool IS_EOL(char value)   { return (value == '\r') || (value == '\n'); }
inline static bool IS_DIGIT(char value) { return (value >= '0')  && (value <= '9'); }

static cstring skip_blank(cstring line) {
	while (IS_BLANK(*line)) { ++line; }
	return line;
}

static s8 parse_sign(cstring line, cstring * next_out) {
	if (*line == '-') { *next_out = line + 1; return -1; }
	if (*line == '+') { *next_out = line + 1; return  1; }
	*next_out = line; return 1;
}

inline static u32 parse_u32(u32 value, cstring line, cstring * next_out) {
	// @Note: no overflow protection
	while (IS_DIGIT(*line)) {
		value = value * 10 + (*line - '0');
		++line;
	}
	*next_out = line; return value;
}

static s32 parse_s32(cstring line, cstring * next_out) {
	s8 sign = parse_sign(skip_blank(line), &line);
	u32 value = parse_u32(0u, line, next_out);
	return sign * (s32)value;
}

inline static r32 construct_r32(s8 sign, u32 mantissa, s32 exponent_10) {
	// ldexp(a, b) == a * 2^b
	// return sign * (mantissa * powf(10, (r32)exponent));
	// return sign * ldexpf(mantissa * powf(5, (r32)exponent), exponent);

	if (!mantissa) { return 0.0f; }
	if (!exponent_10) { return sign * (r32)mantissa; }

	// > mantissa_x * 2^exponent_2 == mantissa * 10^exponent_10
	// > [start with] exponent_2 == exponent_10
	// > [calculate]  mantissa_x == mantissa * 5^exponent_10
	// > preserve significance by transferring powers of 2 from mantissa to exponent_2
	s32 exponent_2 = exponent_10;

	while (exponent_10 > 0) {
		constexpr int const mask = 0b11100000000000000000000000000000;
		while (mantissa & mask) { mantissa >>= 1u; ++exponent_2; }
		mantissa *= 5u; --exponent_10;
	}

	while (exponent_10 < 0) {
		constexpr int const mask = 0b10000000000000000000000000000000;
		while (!(mantissa & mask)) { mantissa <<= 1u; --exponent_2; }
		mantissa /= 5u; ++exponent_10;
	}

	return sign * ldexpf((r32)mantissa, exponent_2);
}

static r32 parse_r32(cstring line, cstring * next_out) {
	s8 sign = parse_sign(skip_blank(line), &line);

	// @Note: no digits count limit
	u32 mantissa = parse_u32(0u, line, &line);
	s32 exponent = 0;

	if (*line == '.') {
		cstring line_before = ++line;
		mantissa = parse_u32(mantissa, line, &line);
		exponent -= (s32)(line - line_before);
	}

	if (*line == 'e' || *line == 'E') {
		exponent += parse_s32(line + 1, &line);
	}

	*next_out = line;
	return construct_r32(sign, mantissa, exponent);
}

struct face_index { s32 v, t, n; };
static face_index parse_face_index(cstring line, cstring * next_out) {
	s32 v = parse_s32(line, &line);
	if (*line != '/') { // format: v
		*next_out = line; return {v, 0, 0};
	}

	if (*(++line) == '/') { // format: v//vn
		s32 vn = parse_s32(line + 1, &line);
		*next_out = line; return {v, 0, vn};
	}

	s32 vt = parse_s32(line, &line);
	if (*line != '/') { // format: v/vt
		*next_out = line; return {v, vt, 0};
	}

	// format: v/vt/vn
	s32 vn = parse_s32(line + 1, &line);
	*next_out = line; return {v, vt, vn};
}

static void parse_vec2_line(cstring line, Array<vec2> & array) {
	array.push(); vec2 & item = array.data[array.count - 1];
	item.x = parse_r32(line, &line);
	item.y = parse_r32(line, &line);
}

static void parse_vec3_line(cstring line, Array<vec3> & array) {
	array.push(); vec3 & item = array.data[array.count - 1];
	item.x = parse_r32(line, &line);
	item.y = parse_r32(line, &line);
	item.z = parse_r32(line, &line);
}

inline static u32 translate_face_index(s32 value, u32 base) {
	return (value > 0) ? (value - 1) : (base + value);
}

struct tri_index { u32 v, t, n; };
static void parse_face_line(cstring line, Array<tri_index> & array, u32 vcount, u32 tcount, u32 ncount) {
	Array_Fixed<tri_index, 4> face;

	while (!IS_EOL(*line)) {
		face_index fi = parse_face_index(line, &line);
		face.push({
			translate_face_index(fi.v, vcount),
			translate_face_index(fi.t, tcount),
			translate_face_index(fi.n, ncount),
		});
	}

	// @Note: triangulate CCW
	for (u16 i = 2; i < face.count; ++i) {
		array.push(face.data[i]);
		array.push(face.data[i - 1]);
		array.push(face.data[0]);
	}
}

static void parse(Array<u8> & file, Array<u8> & vertex_attributes, Array<r32> & vertices, Array<u32> & indices) {
	u32 read_i;

	// @Note: prepare packed buffers
	u32 count_buffer_v  = 0;
	u32 count_buffer_vt = 0;
	u32 count_buffer_vn = 0;
	u32 count_buffer_f  = 0;

	read_i = 0;
	while (read_i < file.count) {
		cstring line = skip_blank((cstring)(file.data + read_i));
		switch (*line) {
			case 'v': switch (*(line + 1)) {
				case ' ': ++count_buffer_v; break;
				case 't': ++count_buffer_vt; break;
				case 'n': ++count_buffer_vn; break;
			} break;

			case 'f': ++count_buffer_f; break;
		}
		cmemory eol = memchr((cmemory)line, '\n', file.count - read_i);
		read_i = eol ? (u32)((u8 *)eol - file.data) + 1 : file.count;
	}

	Array<vec3> packed_v(count_buffer_v);
	Array<vec2> packed_vt(count_buffer_vt);
	Array<vec3> packed_vn(count_buffer_vn);
	Array<tri_index> packed_tris(count_buffer_f * 3 * 2);

	// @Note: read packed data
	read_i = 0;
	while (read_i < file.count) {
		cstring line = skip_blank((cstring)(file.data + read_i));
		switch (*line) {
			case 'v': switch (*(line + 1)) {
				case ' ': parse_vec3_line(line + 2, packed_v); break;
				case 't': parse_vec2_line(line + 3, packed_vt); break;
				case 'n': parse_vec3_line(line + 3, packed_vn); break;
			} break;

			case 'f': {
				parse_face_line(
					line + 2, packed_tris,
					packed_v.count, packed_vt.count, packed_vn.count
				);
			} break;
		}
		cmemory eol = memchr((cmemory)line, '\n', file.count - read_i);
		read_i = eol ? (u32)((u8 *)eol - file.data) + 1 : file.count;
	}

	file.set_capacity(0);

	// @Note: unpack vertices
	#define PUSH_STRIDE_IMPL(array) if (array.count > 0) {\
		u8 elements_count = (u8)C_ARRAY_LENGTH(array.data[0].data);\
		elements_per_vertex += elements_count;\
		vertex_attributes.push(elements_count);\
	}
	u32 elements_per_vertex = 0;
	vertex_attributes.set_capacity(3);
	PUSH_STRIDE_IMPL(packed_v);
	PUSH_STRIDE_IMPL(packed_vt);
	PUSH_STRIDE_IMPL(packed_vn);
	#undef PUSH_STRIDE_IMPL

	vertices.set_capacity(count_buffer_f * 3 * elements_per_vertex);
	indices.set_capacity(count_buffer_f * 3);

	// u64 ticks_before = custom::timer::get_ticks();
	Array<u32> packed_indices(packed_tris.capacity, packed_tris.capacity);
	u32 next_index = 0;
	for (u32 i = 0; i < packed_tris.count; ++i) {
		tri_index fi = packed_tris.data[i];
		u32 index = UINT32_MAX;

		// @Optimize: - suzanne.obj: 22500 ticks instead of 900 (25x slower)
		for (u32 prev = 0; prev < i; ++prev) {
			tri_index prev_fi = packed_tris.data[prev];
			if (fi.v != prev_fi.v) { continue; }
			if (fi.t != prev_fi.t) { continue; }
			if (fi.n != prev_fi.n) { continue; }
			fi = prev_fi;
			index = packed_indices.data[prev];
			break;
		}

		if (index == UINT32_MAX) {
			index = next_index++;
			packed_indices.data[i] = index;
			#define PUSH_VERTEX_IMPL(array, i) if (array.count > 0) {\
				vertices.push_range(array.data[i].data, C_ARRAY_LENGTH(array.data[i].data));\
			}
			PUSH_VERTEX_IMPL(packed_v, fi.v);
			PUSH_VERTEX_IMPL(packed_vt, fi.t);
			PUSH_VERTEX_IMPL(packed_vn, fi.n);
			#undef PUSH_VERTEX_IMPL
		}

		indices.push(index);
	}
	// CUSTOM_MESSAGE("unpack: %lld", (custom::timer::get_ticks() - ticks_before));
}

#undef IS_BLANK
#undef IS_EOL
#undef IS_DIGIT

}}
