#pragma once
#include "engine/core/types.h"

namespace custom {

// @Note: too OpenGL-specific, I suppose?
enum struct Context_Api : u8
{
	OpenGL,
	OpenGL_ES,
};

struct Context_Settings
{
	Context_Api api = Context_Api::OpenGL;
	int  major_version = 4;
	int  minor_version = 6;
	int  robustness = 0; // 0 disabled, 1 enabled (no errors), 2 enabled (with errors)
	//
	bool flush_on_release = false;
	bool legacy_context = false;
	bool legacy_profile = false;
	bool debug = false;
	bool opengl_no_error = false;
};

struct Pixel_Format
{
	int  id;
	//
	int  red_bits = 8;
	int  green_bits = 8;
	int  blue_bits = 8;
	int  alpha_bits = 8;
	//
	int  depth_bits = 24;
	int  stencil_bits = 8;
	//
	bool doublebuffer = true;
	int  swap = 1; // 0 any, 1 exchange, 2 copy
};

extern Context_Settings context_settings;
extern Pixel_Format pixel_format_hint;

/*
struct Pixel_Format_Aux
{
	int  red_shift;
	int  green_shift;
	int  blue_shift;
	int  alpha_shift;
	//
	int  accum_red_bits;
	int  accum_green_bits;
	int  accum_blue_bits;
	int  accum_alpha_bits;
	//
	int  aux_buffers;
	int  samples;
	//
	bool stereo;
	// bool sRGB;
	// bool transparent;
};
*/

}
