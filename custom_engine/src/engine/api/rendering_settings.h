#pragma once
#include "engine/core/types.h"
#include "engine/core/meta.h"

namespace custom {

// @Note: too OpenGL-specific, I suppose?
enum struct Context_Api : u8
{
	OpenGL,
	OpenGL_ES,
};
UNDERLYING_TYPE_META(Context_Api, u8)
IS_ENUM_META(Context_Api)

struct Context_Settings
{
	Context_Api api;
	int  major_version;
	int  minor_version;
	int  robustness; // 0 disabled, 1 enabled (no errors), 2 enabled (with errors)
	//
	bool flush_on_release;
	bool legacy_context;
	bool legacy_profile;
	bool debug;
	bool opengl_no_error;
};

struct Pixel_Format
{
	int  id;
	//
	int  red_bits;
	int  green_bits;
	int  blue_bits;
	int  alpha_bits;
	//
	int  depth_bits;
	int  stencil_bits;
	//
	bool doublebuffer;
};

extern Context_Settings context_settings;
extern Pixel_Format pixel_format_hint;

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

}
