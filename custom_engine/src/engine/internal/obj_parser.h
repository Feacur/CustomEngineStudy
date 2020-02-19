#include "engine/api/timer.h"

// https://github.com/syoyo/tinyobjloader/blob/master/tiny_obj_loader.h
// https://github.com/rlk/obj/blob/master/obj.c
// https://github.com/Oblomov/faster-parser/blob/master/parse-float.h


namespace custom {
namespace obj {

#define IS_BLANK(value) (((value) == ' ') || ((value) == '\t'))
#define IS_EOL(value) (((value) == '\r') || ((value) == '\n'))
#define IS_DIGIT(value) (((value) >= '0') && ((value) <= '9'))

static void skip_blank(cstring line, cstring * next_out) {
	while (IS_BLANK(*line)) { ++line; }
	*next_out = line;
}

static u32 translate_index(s32 value, u32 base) {
	if (value > 0) { return value - 1; }
	if (value == 0) { return 0; }
	return base + value;
}

#include <math.h>

static s8 parse_sign(cstring line, cstring * next_out) {
	s8 value;
	if (*line == '-') {      ++line; value = -1; }
	else if (*line == '+') { ++line; value =  1; }
	else {                           value =  1; }
	*next_out = line;
	return value;
}

static s32 parse_s32(cstring line, cstring * next_out) {
	skip_blank(line, &line);
	s8 sign = parse_sign(line, &line);

	s32 value = 0;
	char c = *line;
	while (IS_DIGIT(c)) {
		value = value * 10 + (c - '0');
		c = *(++line);
	}

	*next_out = line;
	return sign * value;
}

static r32 parse_r32(cstring line, cstring * next_out) {
	skip_blank(line, &line);
	s8 sign = parse_sign(line, &line);

	// @Todo: u64 mantissa and a larger digits limit?
	u32 mantissa = 0;
	s32 exponent = 0;
	u8 digits_limit = 7;

	char c = *line;
	while (IS_DIGIT(c)) {
		if (digits_limit) {
			u32 n = mantissa * 10 + (c - '0');
			if (n > mantissa) { --digits_limit; }
			mantissa = n;
		} else {
			// @Note: compensate for loss of significance; fractional part is ignored in this case
			++exponent; // m * 10^e
		}
		c = *(++line);
	}

	if (c == '.') {
		c = *(++line);
		while (IS_DIGIT(c)) {
			if (digits_limit) {
				u32 n = mantissa * 10 + (c - '0');
				if (n > mantissa) { --digits_limit; }
				mantissa = n;
				--exponent; // m * 10^e
			}
			c = *(++line);
		}
	}

	if (c == 'e' || c == 'E') {
		exponent += parse_s32(line + 1, &line);
	}

	*next_out = line;

	// @Note: both manstissa and exponent are base 10, so the
	//        resulting value = mantissa * 10^exponent
	//        - x^n * y^n = (x*y)^n
	//        - ldexp(a, b) = a * 2^b
	//        - ldexp(a * 5^b, b) = (a * 5^b) * 2^b = a * (5*2)^b = a * 10^b
	if (!mantissa) { return 0; }
	if (!exponent) { return (r32)mantissa; }

	// @Note: basically this happens
	// return sign * (mantissa * powf(10, (r32)exponent));
	// return sign * ldexpf(mantissa * powf(5, (r32)exponent), exponent);

	// @Note: figure out mantissa2 and exponent2
	//        - mantissa2 * 2^exponent2 ~~~~ mantissa1 * 10^exponent1
	//        - start with [exponent2 = exponent1]
	//          compute [mantissa2 ~~~~ mantissa1 * 5^exponent1] by division or multiplication
	//        - preserve significance by correcting mantissa1 and exponent2
	//          mantissa2 ~~~~ mantissa1 * 5^exponent1 * 2^(exponent1 - exponent2)
	s32 exponent2 = exponent;
	if (exponent > 0) {
		// multiply mantissa1 by 5 exponent1 times
		while (exponent > 0) {
			// @Note: correct mantissa1 before multiplication by
			//        keeping 3 most significant bits intact
			//        - closest larger power of two for 5 is 2^3
			//        0b 1110 0000  0000 0000  0000 0000  0000 0000
			while (mantissa & 0xe0000000) {
				mantissa >>= 1; ++exponent2;
			}
			//
			mantissa *= 5; --exponent;
		}
	}
	else {
		// divide mantissa1 by 5 exponent1 times
		while (exponent < 0) {
			// @Note: correct mantissa1 before division by
			//        making use of most significant bits
			//        0b 1000 0000  0000 0000  0000 0000  0000 0000
			while (!(mantissa & 0x80000000)) {
				mantissa <<= 1; --exponent2;
			}
			//
			mantissa /= 5; ++exponent;
		}
	}
	return sign * ldexpf((r32)mantissa, exponent2);
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
	array.push(); vec2 & item = array[array.count - 1];
	item.x = parse_r32(line, &line);
	item.y = parse_r32(line, &line);
}

static void parse_vec3_line(cstring line, Array<vec3> & array) {
	array.push(); vec3 & item = array[array.count - 1];
	item.x = parse_r32(line, &line);
	item.y = parse_r32(line, &line);
	item.z = parse_r32(line, &line);
}

struct tri_index { u32 v, t, n; u32 index; };
static void parse_face_line(cstring line, Array<tri_index> & array, u32 vcount, u32 tcount, u32 ncount) {
	Array<tri_index> face(4);

	while (!IS_EOL(*line)) {
		face_index fi = parse_face_index(line, &line);
		face.push({
			translate_index(fi.v, vcount),
			translate_index(fi.t, tcount),
			translate_index(fi.n, ncount),
			UINT32_MAX
		});
		skip_blank(line, &line);
	}

	// @Note: triangulate CCW
	for (u32 i = 1, last = face.count - 1; i < last; ++i) {
		array.push(face[i + 1]);
		array.push(face[i]);
		array.push(*face.data);
	}
}

static void parse(Array<u8> & file, Array<u8> & vertex_attributes, Array<r32> & vertices, Array<u32> & indices) {
	u32 read_i;

	// @Note: prepare packed buffers
	// u64 prepare1 = custom::timer::get_ticks();
	u32 count_buffer_v   = 0;
	u32 count_buffer_vt  = 0;
	u32 count_buffer_vn  = 0;
	u32 count_buffer_f   = 0;

	read_i = 0;
	while (read_i < file.count) {
		cstring line = (cstring)(file.data + read_i);

		skip_blank(line, &line);
		if (*line == 'v') {
			if (line[1] == ' ') {
				++count_buffer_v;
			}
			if (line[1] == 't' && line[2] == ' ') {
				++count_buffer_vt;
			}
			if (line[1] == 'n' && line[2] == ' ') {
				++count_buffer_vn;
			}
		}
		else if (*line == 'f' && line[1] == ' ') {
			++count_buffer_f;
		}

		cmemory eol = memchr((cmemory)line, '\n', file.count - read_i);
		read_i = eol ? (u32)((u8 *)eol - file.data) + 1 : file.count;
	}

	Array<vec3> packed_v(count_buffer_v);
	Array<vec2> packed_vt(count_buffer_vt);
	Array<vec3> packed_vn(count_buffer_vn);
	Array<tri_index> packed_tris(count_buffer_f * 3 * 2);
	// CUSTOM_MESSAGE("prepare:   %f", (float)(custom::timer::get_ticks() - prepare1) / custom::timer::ticks_per_second);

	// @Note: read packed data
	u64 parse_v  = 0;
	u64 parse_vt = 0;
	u64 parse_vn = 0;
	u64 parse_f  = 0;
	// u64 parse1 = custom::timer::get_ticks();
	read_i = 0;
	while (read_i < file.count) {
		cstring line = (cstring)(file.data + read_i);

		// u64 t1 = custom::timer::get_ticks();
		skip_blank(line, &line);
		if (*line == 'v') {
			if (line[1] == ' ') {
				parse_vec3_line(line + 2, packed_v);
				// parse_v += custom::timer::get_ticks() - t1;
			}
			if (line[1] == 't' && line[2] == ' ') {
				parse_vec2_line(line + 3, packed_vt);
				// parse_vt += custom::timer::get_ticks() - t1;
			}
			if (line[1] == 'n' && line[2] == ' ') {
				parse_vec3_line(line + 3, packed_vn);
				// parse_vn += custom::timer::get_ticks() - t1;
			}
		}
		else if (*line == 'f' && line[1] == ' ') {
			parse_face_line(
				line + 2, packed_tris,
				packed_v.count, packed_vt.count, packed_vn.count
			);
			// parse_f += custom::timer::get_ticks() - t1;
		}

		cmemory eol = memchr((cmemory)line, '\n', file.count - read_i);
		read_i = eol ? (u32)((u8 *)eol - file.data) + 1 : file.count;
	}
	// CUSTOM_MESSAGE("read all:  %f", (float)(custom::timer::get_ticks() - parse1) / custom::timer::ticks_per_second);
	// CUSTOM_MESSAGE("- part v:  %f", (float)parse_v / custom::timer::ticks_per_second);
	// CUSTOM_MESSAGE("- part vt: %f", (float)parse_vt / custom::timer::ticks_per_second);
	// CUSTOM_MESSAGE("- part vn: %f", (float)parse_vn / custom::timer::ticks_per_second);
	// CUSTOM_MESSAGE("- part f:  %f", (float)parse_f / custom::timer::ticks_per_second);

	file.set_capacity(0);

	// @Note: unpack vertices
	// u64 unpack1 = custom::timer::get_ticks();

	u32 elements_per_vertex = 0;
	vertex_attributes.set_capacity(3);
	#define PUSH_STRIDE_IMPL(array) if (array.count > 0) {\
		u8 elements_count = (u8)C_ARRAY_LENGTH(array.data[0].data);\
		elements_per_vertex += elements_count;\
		vertex_attributes.push(elements_count);\
	}
	PUSH_STRIDE_IMPL(packed_v);
	PUSH_STRIDE_IMPL(packed_vt);
	PUSH_STRIDE_IMPL(packed_vn);
	#undef PUSH_STRIDE_IMPL
	vertices.set_capacity(count_buffer_f * 3 * elements_per_vertex);
	indices.set_capacity(count_buffer_f * 3);

	// u32 reused_fi = 0;
	u32 next_index = 0;
	for (u32 i = 0; i < packed_tris.count; ++i) {
		tri_index * fi = packed_tris.data + i;

		// @Optimize: - suzanne.obj:      0.12 instead of 0.0015
		//            - suzanne_tris.obj: 0.12 instead of 0.0015
		for (u32 prev = 0; prev < i; ++prev) {
			tri_index * prev_fi = packed_tris.data + prev;
			if (fi->v != prev_fi->v) { continue; }
			if (fi->t != prev_fi->t) { continue; }
			if (fi->n != prev_fi->n) { continue; }
			fi = prev_fi;
			break;
		}

		if (fi == packed_tris.data + i) {
			fi->index = next_index++;
			#define PUSH_VERTEX_IMPL(array, i) if (array.count > 0) {\
				vertices.push_range(array[i].data, C_ARRAY_LENGTH(array[i].data));\
			}
			PUSH_VERTEX_IMPL(packed_v, fi->v);
			PUSH_VERTEX_IMPL(packed_vt, fi->t);
			PUSH_VERTEX_IMPL(packed_vn, fi->n);
			#undef PUSH_VERTEX_IMPL
		}
		// else { ++reused_fi; }

		indices.push(fi->index);
	}
	// CUSTOM_MESSAGE("unpack:    %f", (float)(custom::timer::get_ticks() - unpack1) / custom::timer::ticks_per_second);
	// CUSTOM_MESSAGE("- reused:  %d", reused_fi);
}

#undef IS_BLANK
#undef IS_EOL
#undef IS_DIGIT

}}
