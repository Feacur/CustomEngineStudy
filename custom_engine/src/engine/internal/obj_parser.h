#include "engine/api/timer.h"

// https://github.com/syoyo/tinyobjloader/blob/master/tiny_obj_loader.h
// https://github.com/rlk/obj/blob/master/obj.c

namespace custom {
namespace obj {

#define IS_BLANK(value) (((value) == ' ') || ((value) == '\t'))
#define IS_EOL(value) (((value) == '\r') || ((value) == '\n'))

static void skip_blank(cstring * line) {
	while (IS_BLANK((*line)[0])) { ++(*line); }
}

static u32 translate_index(s32 value, u32 base) {
	if (value > 0) { return value - 1; }
	if (value == 0) { return 0; }
	return base + value;
}

struct face_index
{
	u32 v, t, n;
	u32 index;
};

static void parse_vec2(cstring line, Array<vec2> & array) {
	array.push(); vec2 & item = array[array.count - 1];
	skip_blank(&line);
	if (sscanf(line, "%f %f", &item.x, &item.y) < 2) {
		CUSTOM_ASSERT(false, "failed to parse");
	}
}

static void parse_vec3(cstring line, Array<vec3> & array) {
	array.push(); vec3 & item = array[array.count - 1];
	skip_blank(&line);
	if (sscanf(line, "%f %f %f", &item.x, &item.y, &item.z) < 3) {
		CUSTOM_ASSERT(false, "failed to parse");
	}
}

static void parse_faces(cstring line, Array<face_index> & array, u32 vcount, u32 tcount, u32 ncount) {
	Array<face_index> face(4);

	skip_blank(&line);
	while (!IS_EOL(line[0])) {
		s32 chars_count;
		s32 vi, ti, ni;
		if (sscanf(line, "%d/%d/%d%n", &vi, &ti, &ni, &chars_count) < 3) {
			CUSTOM_ASSERT(false, "failed to parse");
		}
		face.push({
			translate_index(vi, vcount),
			translate_index(ti, tcount),
			translate_index(ni, ncount),
		});
		skip_blank(&(line += chars_count));
	}

	for (u32 i = 1, last = face.count - 1; i < last; ++i) {
		array.push(face[i + 1]);
		array.push(face[i]);
		array.push(face[0]);
	}
}

static void parse(Array<u8> & file, Array<r32> & vertices, Array<u32> & indices) {
	u32 read_i;

	// @Note: prepare packed buffers
	u64 prepare1 = custom::timer::get_ticks();
	u32 count_buffer_v   = 0;
	u32 count_buffer_vt  = 0;
	u32 count_buffer_vn  = 0;
	u32 count_buffer_f   = 0;

	read_i = 0;
	while (read_i < file.count) {
		cstring line = (cstring)(file.data + read_i);
		skip_blank(&line);

		if (line[0] == 'v') {
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
		else if (line[0] == 'f' && line[1] == ' ') {
			++count_buffer_f;
		}

		cmemory eol = memchr((void *)line, '\n', file.count - read_i);
		read_i = eol ? (u32)((u8 *)eol - file.data) + 1 : file.count;
	}

	Array<vec3> packed_v(count_buffer_v);
	Array<vec2> packed_vt(count_buffer_vt);
	Array<vec3> packed_vn(count_buffer_vn);
	Array<face_index> packed_tris(count_buffer_f * 3 * 2);
	CUSTOM_MESSAGE("prepare:   %f", (float)(custom::timer::get_ticks() - prepare1) / custom::timer::ticks_per_second);

	// @Optimize: - suzanne.obj:      0.8
	//            - suzanne_tris.obj: 0.11
	// @Note: read packed data
	u64 parse_v  = 0;
	u64 parse_vt = 0;
	u64 parse_vn = 0;
	u64 parse_f  = 0;
	u64 parse1 = custom::timer::get_ticks();
	read_i = 0;
	while (read_i < file.count) {
		cstring line = (cstring)(file.data + read_i);
		skip_blank(&line);

		u64 t1 = custom::timer::get_ticks();
		if (line[0] == 'v') {
			if (line[1] == ' ') {
				parse_vec3(line + 2, packed_v);
				parse_v += custom::timer::get_ticks() - t1;
			}
			if (line[1] == 't' && line[2] == ' ') {
				parse_vec2(line + 3, packed_vt);
				parse_vt += custom::timer::get_ticks() - t1;
			}
			if (line[1] == 'n' && line[2] == ' ') {
				parse_vec3(line + 3, packed_vn);
				parse_vn += custom::timer::get_ticks() - t1;
			}
		}
		else if (line[0] == 'f' && line[1] == ' ') {
			parse_faces(
				line + 2, packed_tris,
				packed_v.count, packed_vt.count, packed_vn.count
			);
			parse_f += custom::timer::get_ticks() - t1;
		}

		cmemory eol = memchr((void *)line, '\n', file.count - read_i);
		read_i = eol ? (u32)((u8 *)eol - file.data) + 1 : file.count;
	}
	CUSTOM_MESSAGE("read all:  %f", (float)(custom::timer::get_ticks() - parse1) / custom::timer::ticks_per_second);
	CUSTOM_MESSAGE("- part v:  %f", (float)parse_v / custom::timer::ticks_per_second);
	CUSTOM_MESSAGE("- part vt: %f", (float)parse_vt / custom::timer::ticks_per_second);
	CUSTOM_MESSAGE("- part vn: %f", (float)parse_vn / custom::timer::ticks_per_second);
	CUSTOM_MESSAGE("- part f:  %f", (float)parse_f / custom::timer::ticks_per_second);

	file.set_capacity(0);

	// @Note: unpack vertices
	u32 reused_fi = 0;
	u64 unpack1 = custom::timer::get_ticks();
	vertices.set_capacity(count_buffer_f * 3 * 3);
	indices.set_capacity(count_buffer_f * 3);
	u32 next_index = 0;
	for (u32 i = 0; i < packed_tris.count; ++i) {
		face_index * fi = packed_tris.data + i;

		// @Optimize: - suzanne.obj:      0.12 instead of 0.0015
		//            - suzanne_tris.obj: 0.12 instead of 0.0015
		for (u32 prev = 0; prev < i; ++prev) {
			face_index * prev_fi = packed_tris.data + prev;
			if (fi->v != prev_fi->v) { continue; }
			if (fi->t != prev_fi->t) { continue; }
			if (fi->n != prev_fi->n) { continue; }
			fi = prev_fi;
			break;
		}

		if (fi == packed_tris.data + i) {
			fi->index = next_index++;
			vertices.push_range(packed_v[fi->v].data, 3);
			vertices.push_range(packed_vt[fi->t].data, 2);
			vertices.push_range(packed_vn[fi->n].data, 3);
		}
		else { ++reused_fi; }

		indices.push(fi->index);
	}
	CUSTOM_MESSAGE("unpack:    %f", (float)(custom::timer::get_ticks() - unpack1) / custom::timer::ticks_per_second);
	CUSTOM_MESSAGE("- reused:  %d", reused_fi);
}

#undef IS_BLANK
#undef IS_EOL

}}
