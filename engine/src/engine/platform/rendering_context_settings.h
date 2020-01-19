#pragma once
#include "engine/core/code.h"

namespace custom
{
	enum struct Context_Api
	{
		OpenGL,
		OpenGL_ES,
	};

	struct Context_Settings
	{
		Context_Api api;
		int  forward;
		int  profile_bit; // WGL_CONTEXT_CORE_PROFILE_BIT_ARB, WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB
		//
		int  major_version;
		int  minor_version;
		int  robustness; // WGL_NO_RESET_NOTIFICATION_ARB, WGL_LOSE_CONTEXT_ON_RESET_ARB
		int  release_behaviour; // WGL_CONTEXT_RELEASE_BEHAVIOR_NONE_ARB, WGL_CONTEXT_RELEASE_BEHAVIOR_FLUSH_ARB
		//
		bool debug;
		bool opengl_no_error;
	};

	struct Pixel_Format
	{
		uptr id;
		//
		int  redBits;
		int  greenBits;
		int  blueBits;
		int  alphaBits;
		//
		int  depthBits;
		int  stencilBits;
		//
		bool doublebuffer;
	};

	struct Pixel_Format_Aux
	{
		int  redShift;
		int  greenShift;
		int  blueShift;
		int  alphaShift;
		//
		int  accumRedBits;
		int  accumGreenBits;
		int  accumBlueBits;
		int  accumAlphaBits;
		//
		int  auxBuffers;
		int  samples;
		//
		bool stereo;
		// bool sRGB;
		// bool transparent;
	};
}
