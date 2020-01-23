#include "custom_pch.h"
#include "engine/api/graphics_vm.h"
#include "engine/impl/command_buffer.h"

#if !defined(CUSTOM_PRECOMPILED_HEADER)
	#include <glad/glad.h>
#endif

// https://www.khronos.org/registry/OpenGL/index_gl.php

// https://github.com/etodd/lasercrabs/blob/master/src/platform/glvm.cpp

typedef GLchar const * glstring;

static void opengl_message_callback(
	unsigned source, unsigned type, unsigned id, unsigned severity,
	int length, cstring message, void const * userParam
);

//
// API implementation
//

namespace custom {

Graphics_VM::Graphics_VM()
{
	GLint versionMajor;
	glGetIntegerv(GL_MAJOR_VERSION, &versionMajor);
	GLint versionMinor;
	glGetIntegerv(GL_MINOR_VERSION, &versionMinor);

	#if !defined(GES_SHIPPING)
	if (versionMajor == 4 && versionMinor >= 3 || versionMajor > 4) {
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(opengl_message_callback, nullptr);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_FALSE);
	}
	#endif

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_DEPTH_TEST);
	// glDepthFunc(GL_LESS);
	// glDepthRangef(0.0f, 1.0f);
	// glClearDepth(1.0f);

	// glEnable(GL_CULL_FACE);
	// glFrontFace(GL_CCW);
	// glCullFace(GL_BACK);
}

Graphics_VM::~Graphics_VM() = default;

void Graphics_VM::render(Command_Buffer & command_buffer)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	while (command_buffer.offset < command_buffer.bytecode.count) {
		Graphics_Instruction instruction = *command_buffer.read<Graphics_Instruction>();

		if (instruction == Graphics_Instruction::None) {
			CUSTOM_ASSERT(false, "null instruction encountered");
		}

		if (instruction == Graphics_Instruction::Last) {
			CUSTOM_ASSERT(false, "non-instruction encountered");
		}

		if (instruction == Graphics_Instruction::Print_Pointer) {
			cstring message = *command_buffer.read<cstring>();
			CUSTOM_MESSAGE("print pointer: %s", message);
			continue;
		}

		if (instruction == Graphics_Instruction::Print_Inline) {
			u32 length = *command_buffer.read<u32>();
			cstring message = command_buffer.read<char const>(length);
			CUSTOM_MESSAGE("print inline: %d %s", length, message);
			continue;
		}
		
		CUSTOM_ASSERT(false, "unknown instruction encountered: %d", instruction);
	}
}

}

//
// platform implementation
//

#if !defined(GES_SHIPPING)
static void opengl_message_callback(
	GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	glstring message,
	void const * userParam)
{
	// https://www.khronos.org/opengl/wiki/Debug_Output
	// https://www.khronos.org/opengl/wiki/OpenGL_Error

	cstring source_string = NULL;
	switch (source)
	{
		case GL_DEBUG_SOURCE_API:             source_string = "API"; break;
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   source_string = "window system"; break;
		case GL_DEBUG_SOURCE_SHADER_COMPILER: source_string = "shader compiler"; break;
		case GL_DEBUG_SOURCE_THIRD_PARTY:     source_string = "thirdparty"; break;
		case GL_DEBUG_SOURCE_APPLICATION:     source_string = "application"; break;
		case GL_DEBUG_SOURCE_OTHER:           source_string = "other"; break;
		default:                              source_string = "unknown"; break;
	}

	cstring type_string = NULL;
	switch (type)
	{
		case GL_DEBUG_TYPE_ERROR:               type_string = "error"; break;
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: type_string = "deprecated behavior"; break;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  type_string = "undefined behavior"; break;
		case GL_DEBUG_TYPE_PORTABILITY:         type_string = "portability"; break;
		case GL_DEBUG_TYPE_PERFORMANCE:         type_string = "performance"; break;
		case GL_DEBUG_TYPE_MARKER:              type_string = "marker"; break;
		case GL_DEBUG_TYPE_PUSH_GROUP:          type_string = "push group"; break;
		case GL_DEBUG_TYPE_POP_GROUP:           type_string = "pop group"; break;
		case GL_DEBUG_TYPE_OTHER:               type_string = "other"; break;
		default:                                type_string = "unknown"; break;
	}

	cstring severity_string = NULL;
	switch (severity)
	{
		case GL_DEBUG_SEVERITY_HIGH:         severity_string = "high"; break;         // All OpenGL Errors, shader compilation/linking errors, or highly-dangerous undefined behavior
		case GL_DEBUG_SEVERITY_MEDIUM:       severity_string = "medium"; break;       // Major performance warnings, shader compilation/linking warnings, or the use of deprecated functionality
		case GL_DEBUG_SEVERITY_LOW:          severity_string = "low"; break;          // Redundant state change performance warning, or unimportant undefined behavior
		case GL_DEBUG_SEVERITY_NOTIFICATION: severity_string = "notification"; break; // Anything that isn't an error or performance issue.
		default:                             severity_string = "unknown"; break;
	}


	CUSTOM_MESSAGE(
		"OpenGL message:"
		"\n  %s"
		"\n  - type:     %s"
		"\n  - severity: %s"
		"\n  - source:   %s",
		message,
		type_string,
		severity_string,
		source_string
	);
}
#endif
