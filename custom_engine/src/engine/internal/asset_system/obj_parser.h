// #include "engine/api/platform/timer.h"

// https://github.com/syoyo/tinyobjloader/blob/master/tiny_obj_loader.h
// https://github.com/rlk/obj/blob/master/obj.c
// https://github.com/Oblomov/faster-parser/blob/master/parse-float.h

#include <math.h>

namespace custom {
namespace obj {

struct face_index { s32 v, t, n; };
static face_index parse_face_index(cstring * source) {
	s32 v = parse_s32(source);
	if (**source != '/') { // format: v
		return {v, 0, 0};
	}

	++(*source);
	if (**source == '/') { // format: v//vn
		++(*source);
		s32 vn = parse_s32(source);
		return {v, 0, vn};
	}

	s32 vt = parse_s32(source);
	if (**source != '/') { // format: v/vt
		return {v, vt, 0};
	}

	// format: v/vt/vn
	++(*source);
	s32 vn = parse_s32(source);
	return {v, vt, vn};
}

inline static u32 translate_face_index(s32 value, u32 base) {
	return (value > 0) ? (value - 1) : (base + value);
}

struct tri_index { u32 v, t, n; };
static void parse_face_line(cstring source, Array<tri_index> & temporary_buffer, Array<tri_index> & out, u32 vcount, u32 tcount, u32 ncount) {
	temporary_buffer.count = 0;
	while (!IS_EOL(*source)) {
		parse_void(&source);
		face_index fi = parse_face_index(&source);
		temporary_buffer.push({
			translate_face_index(fi.v, vcount),
			translate_face_index(fi.t, tcount),
			translate_face_index(fi.n, ncount),
		});
	}

	// @Note: triangulate CCW
	for (u16 i = 2; i < temporary_buffer.count; ++i) {
		out.push(temporary_buffer.data[i]);
		out.push(temporary_buffer.data[i - 1]);
		out.push(temporary_buffer.data[0]);
	}
}

static void parse(Array<u8> const & file, Array<u8> & vertex_attributes, Array<r32> & vertices, Array<u32> & indices) {
	cstring source;

	// @Note: prepare packed buffers
	u32 count_buffer_v  = 0;
	u32 count_buffer_vt = 0;
	u32 count_buffer_vn = 0;
	u32 count_buffer_f  = 0;

	source = (cstring)file.data;
	while (*source) {
		switch (*source) {
			case 'v': ++source; switch (*source) {
				case ' ': ++count_buffer_v; break;
				case 't': ++count_buffer_vt; break;
				case 'n': ++count_buffer_vn; break;
			} break;

			case 'f': ++count_buffer_f; break;
		}
		to_next_line(&source);
	}

	Array<vec3> packed_v(count_buffer_v);
	Array<vec2> packed_vt(count_buffer_vt);
	Array<vec3> packed_vn(count_buffer_vn);
	Array<tri_index> packed_tris(count_buffer_f * 3 * 2);
	Array<tri_index> temporary_buffer(4);

	// @Note: read packed data
	source = (cstring)file.data;
	while (*source) {
		switch (*source) {
			case 'v': ++source; switch (*source) {
				case ' ':           packed_v.push(parse_vec3(&source)); break;
				case 't': ++source; packed_vt.push(parse_vec2(&source)); break;
				case 'n': ++source; packed_vn.push(parse_vec3(&source)); break;
			} break;

			case 'f': {
				++source;
				parse_face_line(
					source, temporary_buffer, packed_tris,
					packed_v.count, packed_vt.count, packed_vn.count
				);
			} break;
		}
		to_next_line(&source);
	}

	// @Note: unpack vertices
	#define PUSH_STRIDE_IMPL(array)\
		if (array.count > 0) {\
			u8 elements_count = (u8)(sizeof(*array.data) / sizeof(r32));\
			elements_per_vertex += elements_count;\
			vertex_attributes.push(elements_count);\
		}\

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
		u32 index = custom::empty_index;

		for (u32 prev = 0; prev < i; ++prev) {
			// @Optimize
			tri_index prev_fi = packed_tris.data[prev];
			if (fi.v != prev_fi.v) { continue; }
			if (fi.t != prev_fi.t) { continue; }
			if (fi.n != prev_fi.n) { continue; }
			fi = prev_fi;
			index = packed_indices.data[prev];
			break;
		}

		if (index == custom::empty_index) {
			index = next_index++;
			packed_indices.data[i] = index;
			#define PUSH_VERTEX_IMPL(array, i)\
				if (array.count > 0) {\
					vertices.push_range((r32 *)&array.data[i], (sizeof(*array.data) / sizeof(r32)));\
				}\

			PUSH_VERTEX_IMPL(packed_v, fi.v);
			PUSH_VERTEX_IMPL(packed_vt, fi.t);
			PUSH_VERTEX_IMPL(packed_vn, fi.n);
			#undef PUSH_VERTEX_IMPL
		}

		indices.push(index);
	}
	// CUSTOM_TRACE("unpack: %lld", (custom::timer::get_ticks() - ticks_before));
}

}}
