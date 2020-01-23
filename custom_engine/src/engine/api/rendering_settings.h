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
	//
	int  forward;
	int  profile_bit; // WGL_CONTEXT_CORE_PROFILE_BIT_ARB, WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB
	int  robustness; // WGL_NO_RESET_NOTIFICATION_ARB, WGL_LOSE_CONTEXT_ON_RESET_ARB
	int  release_behaviour; // WGL_CONTEXT_RELEASE_BEHAVIOR_NONE_ARB, WGL_CONTEXT_RELEASE_BEHAVIOR_FLUSH_ARB
	//
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
