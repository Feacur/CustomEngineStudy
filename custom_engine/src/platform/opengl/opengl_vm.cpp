#include "custom_pch.h"
#include "engine/core/math_types.h"
#include "engine/api/graphics_vm.h"
#include "engine/impl/command_buffer.h"

#if !defined(CUSTOM_PRECOMPILED_HEADER)
	#include <glad/glad.h>
#endif

// https://www.khronos.org/registry/OpenGL/index_gl.php

// https://github.com/etodd/lasercrabs/blob/master/src/platform/glvm.cpp

typedef GLchar const * glstring;

struct ShaderProps
{
	GLenum   type;
	cstring version;
	cstring defines;
};

//
// API implementation
//

static void consume_single_instruction(custom::Command_Buffer const & command_buffer);

static void opengl_message_callback(
	unsigned source, unsigned type, unsigned id, unsigned severity,
	int length, cstring message, void const * userParam
);
static GLuint create_program(cstring source);

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

void Graphics_VM::render(Command_Buffer const & command_buffer)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	while (command_buffer.offset < command_buffer.bytecode.count) {
		consume_single_instruction(command_buffer);
	}
}

}

//
// instruction implementation
//

static void consume_single_instruction(custom::Command_Buffer const & command_buffer)
{
	custom::Graphics_Instruction instruction = *command_buffer.read<custom::Graphics_Instruction>();
	switch (instruction)
	{
		//
		case custom::Graphics_Instruction::Viewport: {
			CUSTOM_MESSAGE("// @Todo: Viewport");
			ivec2 const * position = command_buffer.read<ivec2>();
			ivec2 const * size     = command_buffer.read<ivec2>();
			// glViewport(x, y, width, height);
		} return;

		//
		case custom::Graphics_Instruction::Prepare_Uniform: {
			CUSTOM_MESSAGE("// @Todo: Prepare_Uniform");
		} return;

		//
		case custom::Graphics_Instruction::Allocate_Shader: {
			CUSTOM_MESSAGE("// @Todo: Allocate_Shader");
			cstring source = *command_buffer.read<cstring>();
			GLuint id = create_program(source);
			// GLint uniform_location = glGetUniformLocation(id, uniform_name);
		} return;

		case custom::Graphics_Instruction::Allocate_Texture: {
			CUSTOM_MESSAGE("// @Todo: Allocate_Texture");
			// GLenum internal_format = GL_RGBA8;
			// glCreateTextures(GL_TEXTURE_2D, 1, &id);
			// glTextureStorage2D(id, 1, internal_format, width, height);
			// glTextureParameteri(id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			// glTextureParameteri(id, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			// glTextureParameteri(id, GL_TEXTURE_WRAP_S, GL_REPEAT);
			// glTextureParameteri(id, GL_TEXTURE_WRAP_T, GL_REPEAT);
		} return;

		case custom::Graphics_Instruction::Allocate_Mesh: {
			CUSTOM_MESSAGE("// @Todo: Allocate_Mesh");
			
			// glGenVertexArrays(1, &id);
			// glBindVertexArray(id);

			// glBindBuffer(GL_ARRAY_BUFFER, id);
			// glCreateBuffers(1, &id);
			// glBindBuffer(GL_ARRAY_BUFFER, id);
			// glBufferData(GL_ARRAY_BUFFER, count * sizeof(float), vertices, GL_STATIC_DRAW);

			// u32 stride = 0;
			// for (auto & element : bufferLayout)
			// {
			// 	stride += element.GetSize();
			// }
			// for (auto const & element : bufferLayout)
			// {
			// 	glEnableVertexAttribArray(vertexAttribIndex);
			// 	glVertexAttribPointer(
			// 		vertexAttribIndex,
			// 		element.GetComponentCount(),
			// 		ShaderDataTypeOpenGLBaseType(element.Type),
			// 		element.Normalized ? GL_TRUE : GL_FALSE,
			// 		stride,
			// 		(void const *)offset
			// 	);
			// 	offset += element.GetSize();
			// 	vertexAttribIndex++;
			// }

			// 
			// glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
			// glCreateBuffers(1, &id);
			// glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
			// glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(u32), indices, GL_STATIC_DRAW);
		} return;

		//
		case custom::Graphics_Instruction::Free_Shader: {
			CUSTOM_MESSAGE("// @Todo: Free_Shader");
			// glDeleteProgram(id);
		} return;

		case custom::Graphics_Instruction::Free_Texture: {
			CUSTOM_MESSAGE("// @Todo: Free_Texture");
			// glDeleteTextures(1, &id);
		} return;

		case custom::Graphics_Instruction::Free_Mesh: {
			CUSTOM_MESSAGE("// @Todo: Free_Mesh");
			// glDeleteBuffers(GL_ARRAY_BUFFER, &id);
			// glDeleteBuffers(GL_ELEMENT_ARRAY_BUFFER, &id);
			// glDeleteVertexArrays(1, &id);
		} return;

		//
		case custom::Graphics_Instruction::Use_Shader: {
			CUSTOM_MESSAGE("// @Todo: Use_Shader");
			// glUseProgram(id);
		} return;

		case custom::Graphics_Instruction::Use_Texture: {
			CUSTOM_MESSAGE("// @Todo: Use_Texture");
			// glBindTextureUnit(slot, id); // load uniform int slot beforehand
		} return;

		case custom::Graphics_Instruction::Use_Mesh: {
			CUSTOM_MESSAGE("// @Todo: Use_Mesh");
			// glBindVertexArray(id);
			// @Todo: potentially rebind indices buffer?
		} return;

		//
		case custom::Graphics_Instruction::Load_Uniform: {
			CUSTOM_MESSAGE("// @Todo: Load_Uniform");
			// glUniformMatrix4fv(location, 1, GL_FALSE, value_pointer);
			// glUniform1f(location, value);
			// glUniform1i(location, value);
			// glUniform1fv(location, count, values_pointer);
			// glUniform1iv(location, count, values_pointer);
		} return;

		case custom::Graphics_Instruction::Load_Texture: {
			CUSTOM_MESSAGE("// @Todo: Load_Texture");
			// GLenum data_format = GL_RGBA;
			// glTextureSubImage2D(id, 0, 0, 0, width, height, data_format, GL_UNSIGNED_BYTE, data);
		} return;

		//
		case custom::Graphics_Instruction::Draw: {
			CUSTOM_MESSAGE("// @Todo: Draw");
			// glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
		} return;

		case custom::Graphics_Instruction::Overlay: {
			CUSTOM_MESSAGE("// @Todo: Overlay");
			// send to a vertex shader indices [0, 1, 2]
			// glDrawArrays(GL_TRIANGLES, 0, 3);
			// https://rauwendaal.net/2014/06/14/rendering-a-screen-covering-triangle-in-opengl/
			// https://twitter.com/nice_byte/status/1093355080235999232
		} return;
	}

	// test
	switch (instruction)
	{
		case custom::Graphics_Instruction::Print_Pointer: {
			cstring message = *command_buffer.read<cstring>();
			CUSTOM_MESSAGE("print pointer: %s", message);
		} return;

		case custom::Graphics_Instruction::Print_Inline: {
			u32 length = *command_buffer.read<u32>();
			cstring message = command_buffer.read<char const>(length);
			CUSTOM_MESSAGE("print inline: %d %s", length, message);
		} return;
	}

	// error
	switch (instruction)
	{
		case custom::Graphics_Instruction::None: {
			CUSTOM_ASSERT(false, "null instruction encountered");
		} return;

		case custom::Graphics_Instruction::Last: {
			CUSTOM_ASSERT(false, "non-instruction encountered");
		} return;
	}

	if (instruction < custom::Graphics_Instruction::Last) {
		CUSTOM_ASSERT(false, "unhandled instruction encountered: %d", instruction);
	}

	CUSTOM_ASSERT(false, "unknown instruction encountered: %d", instruction);
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

static bool verify_compilation(GLuint shader)
{
	GLint is_compiled = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &is_compiled);
	if (is_compiled == GL_TRUE) { return true; }

	// @Note: linker will inform of the errors anyway
	GLint max_length = 0;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &max_length);

	custom::Array<GLchar> info_log(max_length);
	glGetShaderInfoLog(shader, max_length, &max_length, info_log.data);
	CUSTOM_MESSAGE("failed to compile shader:\n%s", info_log.data);

	return false;
}

static bool verify_linking(GLuint program)
{
	GLint is_linked = 0;
	glGetProgramiv(program, GL_LINK_STATUS, (int*)&is_linked);
	if (is_linked == GL_TRUE) { return true; }

	GLint max_length = 0;
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &max_length);

	custom::Array<GLchar> info_log(max_length);
	glGetProgramInfoLog(program, max_length, &max_length, info_log.data);
	CUSTOM_MESSAGE("failed to link program:\n%s", info_log.data);

	return false;
}

static GLuint create_program(cstring source)
{
	// @Todo: read this meta info from outside?
	static ShaderProps compilations_props[] = {
		{ GL_VERTEX_SHADER,   "#version 330 core\n", "#define VERTEX_SECTION\n" },
		{ GL_FRAGMENT_SHADER, "#version 330 core\n", "#define FRAGMENT_SECTION\n" },
		// { GL_GEOMETRY_SHADER, "#version 330 core\n", "#define GEOMETRY_SECTION\n" },
		// { GL_COMPUTE_SHADER,  "#version 430 core\n", "#define COMPUTE_SECTION\n" },
		// { GL_TESS_CONTROL_SHADER,    "#version 400 core\n", "#define TESSELATION_CONTROL_SECTION\n" },
		// { GL_TESS_EVALUATION_SHADER, "#version 400 core\n", "#define TESSELATION_EVALUATION_SECTION\n" },
	};
	u8 const compilations_props_count = C_ARRAY_LENGTH(compilations_props);

	// Compile shaders
	GLuint shader_ids[4] = {};
	for (u8 i = 0; i < compilations_props_count; i++)
	{
		glstring code[] = { compilations_props[i].version, compilations_props[i].defines, source };
		GLuint shader_id = glCreateShader(compilations_props[i].type);
		glShaderSource(shader_id, C_ARRAY_LENGTH(code), code, 0);
		glCompileShader(shader_id);
		shader_ids[i] = shader_id;
	}

	bool is_compiled = true;
	for (u8 i = 0; i < compilations_props_count; i++)
	{
		bool isOk = verify_compilation(shader_ids[i]);
		is_compiled = is_compiled && isOk;
	}

	// Link the program
	GLuint program_id = glCreateProgram();
	for (u8 i = 0; i < compilations_props_count; i++) {
		glAttachShader(program_id, shader_ids[i]);
	}
	glLinkProgram(program_id);
	bool is_linked = verify_linking(program_id);

	// Free shader resources
	for (u8 i = 0; i < compilations_props_count; i++) {
		glDetachShader(program_id, shader_ids[i]);
	}
	for (u8 i = 0; i < compilations_props_count; i++) {
		glDeleteShader(shader_ids[i]);
	}

	if (!is_compiled || !is_linked) {
		glDeleteProgram(program_id);
		return 0;
	}

	return program_id;
}

// static GLenum ShaderDataTypeOpenGLBaseType(ShaderDataType type)
// {
// 	GES_PROFILE_FUNCTION();
// 	switch(type)
// 	{
// 		case ShaderDataType::Float1: return GL_FLOAT;
// 		case ShaderDataType::Float2: return GL_FLOAT;
// 		case ShaderDataType::Float3: return GL_FLOAT;
// 		case ShaderDataType::Float4: return GL_FLOAT;
// 		case ShaderDataType::Mat3:   return GL_FLOAT;
// 		case ShaderDataType::Mat4:   return GL_FLOAT;
// 		case ShaderDataType::Int1:   return GL_INT;
// 		case ShaderDataType::Int2:   return GL_INT;
// 		case ShaderDataType::Int3:   return GL_INT;
// 		case ShaderDataType::Int4:   return GL_INT;
// 		case ShaderDataType::Bool:   return GL_BOOL;
// 	}
// 	GES_CORE_ASSERT(false, "unsupported ShaderDataType '{0}'", (s32)type);
// 	return 0;
// }

// enum class ShaderDataType
// {
// 	None = 0,
// 	Float1,
// 	Float2,
// 	Float3,
// 	Float4,
// 	Mat3,
// 	Mat4,
// 	Int1,
// 	Int2,
// 	Int3,
// 	Int4,
// 	Bool,
// };

// static u32 ShaderDataTypeComponentCount(ShaderDataType type)
// {
// 	switch(type)
// 	{
// 		case ShaderDataType::Float1: return 1;
// 		case ShaderDataType::Float2: return 2;
// 		case ShaderDataType::Float3: return 3;
// 		case ShaderDataType::Float4: return 4;
// 		case ShaderDataType::Mat3:   return 3 * 3;
// 		case ShaderDataType::Mat4:   return 4 * 4;
// 		case ShaderDataType::Int1:   return 1;
// 		case ShaderDataType::Int2:   return 2;
// 		case ShaderDataType::Int3:   return 3;
// 		case ShaderDataType::Int4:   return 4;
// 		case ShaderDataType::Bool:   return 1;
// 	}
// 	// GES_CORE_ASSERT(false, "unsupported ShaderDataType '{0}'", (s32)type);
// 	return 0;
// }

// static u32 ShaderDataTypeSingleSize(ShaderDataType type)
// {
// 	switch(type)
// 	{
// 		case ShaderDataType::Float1: return sizeof(r32);
// 		case ShaderDataType::Float2: return sizeof(r32);
// 		case ShaderDataType::Float3: return sizeof(r32);
// 		case ShaderDataType::Float4: return sizeof(r32);
// 		case ShaderDataType::Mat3:   return sizeof(r32);
// 		case ShaderDataType::Mat4:   return sizeof(r32);
// 		case ShaderDataType::Int1:   return sizeof(s32);
// 		case ShaderDataType::Int2:   return sizeof(s32);
// 		case ShaderDataType::Int3:   return sizeof(s32);
// 		case ShaderDataType::Int4:   return sizeof(s32);
// 		case ShaderDataType::Bool:   return sizeof(s8);
// 	}
// 	// GES_CORE_ASSERT(false, "unsupported ShaderDataType '{0}'", (s32)type);
// 	return 0;
// }
