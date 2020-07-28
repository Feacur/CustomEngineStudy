#include "custom_pch.h"

#include "engine/core/math_types.h"
#include "engine/api/platform/graphics_vm.h"
#include "engine/api/platform/graphics_resource.h"
#include "engine/api/graphics_params.h"
#include "engine/impl/array.h"
#include "engine/impl/array_fixed.h"
#include "engine/impl/bytecode.h"

#if !defined(CUSTOM_PRECOMPILED_HEADER)
	#include <glad/glad.h>
	#include <new>
#endif

#if !defined(CUSTOM_SHIPPING)
	static void PLATFORM_CONSUME_ERRORS();
	static void PLATFORM_INIT_DEBUG();
#else
	#define PLATFORM_CONSUME_ERRORS() (void)0
	#define PLATFORM_INIT_DEBUG() (void)0
#endif

#define COMPILE_VERSION(major, minor) (major * 10 + minor)

// https://www.khronos.org/registry/OpenGL/index_gl.php

// https://github.com/etodd/lasercrabs/blob/master/src/platform/glvm.cpp

// @Todo: improve duplication protection:
//        - 'id' check for Allocate_###
//        - 'uploaded' state check for Load_###
//        - provide default mesh, texture, program

constexpr u32 const empty_gl_id = 0;
constexpr u32 const empty_unit = UINT32_MAX;

typedef GLchar const * glstring;

namespace opengl {

struct Field
{
	u32 id;
	GLint location;
};

struct Program
{
	GLuint id = empty_gl_id;
	bool uploaded = false;
	// custom::Array_Fixed<Field, 4> attributes;
	custom::Array_Fixed<Field, 10> uniforms;

	~Program() {
		id = empty_gl_id;
		uploaded = false;
		// attributes.count = 0;
		uniforms.count = 0;
	}
};
template struct custom::Array<Program>;

struct Texture
{
	GLuint id = empty_gl_id;
	bool uploaded = false;
	b8 is_dynamic;
	GLenum target;
	ivec2 size;
	u8 channels;
	custom::graphics::Data_Type data_type;
	custom::graphics::Texture_Type texture_type;
	custom::graphics::Filter_Mode min_tex, min_mip, mag_tex;
	custom::graphics::Wrap_Mode wrap_x, wrap_y;

	~Texture() {
		id = empty_gl_id;
		uploaded = false;
	}
};
template struct custom::Array<Texture>;

struct Sampler
{
	GLuint id = empty_gl_id;
	custom::graphics::Filter_Mode min_tex, min_mip, mag_tex;
	custom::graphics::Wrap_Mode wrap_x, wrap_y;

	~Sampler() {
		id = empty_gl_id;
	}
};
template struct custom::Array<Sampler>;

struct Attribute
{
	u8 count;
};

struct Buffer
{
	GLuint id = empty_gl_id;
	b8 is_index;
	custom::graphics::Mesh_Frequency frequency;
	custom::graphics::Mesh_Access access;
	custom::graphics::Data_Type type;
	u32 capacity, count;
	custom::Array_Fixed<Attribute, 4> attributes;

	~Buffer() {
		id = empty_gl_id;
		attributes.count = 0;
	}
};

struct Mesh
{
	GLuint id = empty_gl_id;
	bool uploaded = false;
	custom::Array_Fixed<Buffer, 2> buffers;
	u8 index_buffer;

	~Mesh() {
		id = empty_gl_id;
		uploaded = false;
		buffers.count = 0;
	}
};
template struct custom::Array<Mesh>;

struct Render_Buffer
{
	GLuint id = empty_gl_id;
	GLenum target;
	ivec2 size;
	custom::graphics::Data_Type data_type;
	custom::graphics::Texture_Type texture_type;

	~Render_Buffer() {
		id = empty_gl_id;
	}
};

struct Target
{
	GLuint id = empty_gl_id;
	GLenum target;
	custom::Array_Fixed<u32, 2> texture_ids;
	custom::Array_Fixed<Render_Buffer, 1> buffers;

	~Target() {
		id = empty_gl_id;
		texture_ids.count = 0;
		buffers.count = 0;
	}
};
template struct custom::Array<Target>;

struct Data
{
	~Data() = default;

	u32 version;

	// @Note: might store offsets for random search
	custom::Array<char> uniform_names;
	custom::Array<u32>  uniform_names_lengths;

	custom::Array<custom::graphics::unit_id> unit_ids; // sparse
	u32 active_program = empty_asset_id;
	u32 active_mesh    = empty_asset_id;
	u32 active_target  = empty_asset_id;

	custom::Array<Program> programs; // sparse
	custom::Array<Texture> textures; // sparse
	custom::Array<Sampler> samplers; // sparse
	custom::Array<Mesh>    meshes;   // sparse
	custom::Array<Target>  targets;  // sparse

	void programs_ensure_capacity(u32 id) {
		u32 capacity_before = programs.capacity;
		programs.ensure_capacity(id + 1);
		for (u32 i = capacity_before; i < programs.capacity; ++i) {
			(programs.data + i)->~Program();
		}
	}

	void textures_ensure_capacity(u32 id) {
		u32 capacity_before = textures.capacity;
		textures.ensure_capacity(id + 1);
		for (u32 i = capacity_before; i < textures.capacity; ++i) {
			(textures.data + i)->~Texture();
		}
	}

	void samplers_ensure_capacity(u32 id) {
		u32 capacity_before = samplers.capacity;
		samplers.ensure_capacity(id + 1);
		for (u32 i = capacity_before; i < samplers.capacity; ++i) {
			(samplers.data + i)->~Sampler();
		}
	}

	void meshes_ensure_capacity(u32 id) {
		u32 capacity_before = meshes.capacity;
		meshes.ensure_capacity(id + 1);
		for (u32 i = capacity_before; i < meshes.capacity; ++i) {
			(meshes.data + i)->~Mesh();
		}
	}

	void targets_ensure_capacity(u32 id) {
		u32 capacity_before = targets.capacity;
		targets.ensure_capacity(id + 1);
		for (u32 i = capacity_before; i < targets.capacity; ++i) {
			(targets.data + i)->~Target();
		}
	}
};

}

static opengl::Data ogl;

static u32 find_uniform_id(cstring value, u32 value_length) {
	u32 name_offset = 0;
	for (u32 i = 0; i < ogl.uniform_names_lengths.count; ++i) {
		u32 name_length = ogl.uniform_names_lengths[i];
		cstring name = &ogl.uniform_names[name_offset];
		if (strncmp(value, name, value_length) == 0) { return i; }
		name_offset += name_length;
	}
	CUSTOM_ASSERT(false, "failed to find uniform '%s'", value);
	return empty_asset_id;
}

static opengl::Field const * find_uniform_field(u32 program_id, u32 uniform_id) {
	opengl::Program const * program = &ogl.programs.get(program_id);
	for (u16 i = 0; i < program->uniforms.count; ++i) {
		if (program->uniforms[i].id == uniform_id) {
			return &program->uniforms[i];
		}
	}
	CUSTOM_ASSERT(false, "program %d doesn't have uniform %d; or it was optimized out", program_id, uniform_id);
	return NULL;
}

static u32 find_unit(u32 texture, u32 sampler, u32 default_unit) {
	for (u32 i = 0; i < ogl.unit_ids.capacity; ++i) {
		custom::graphics::unit_id const & it = ogl.unit_ids.get(i);
		if (it.texture != texture) { continue; }
		if (it.sampler != sampler) { continue; }
		return i;
	}
	return default_unit;
}

static u32 find_empty_unit(u32 default_unit) {
	for (u32 i = 0; i < ogl.unit_ids.capacity; ++i) {
		custom::graphics::unit_id const & it = ogl.unit_ids.get(i);
		if (it.texture != empty_asset_id) { continue; }
		if (it.sampler != empty_asset_id) { continue; }
		return i;
	}
	return default_unit;
}

//
// API implementation
//

struct Program_Field
{
	GLchar name[32];
	GLsizei name_count;
	GLint size;
	GLenum type;
	GLint location;
};

namespace custom {
namespace graphics {

void init(void) {
	GLint version_major;
	GLint version_minor;
	glGetIntegerv(GL_MAJOR_VERSION, &version_major);
	glGetIntegerv(GL_MINOR_VERSION, &version_minor);
	ogl.version = COMPILE_VERSION(version_major, version_minor);

	PLATFORM_INIT_DEBUG();

	GLint max_combined_texture_image_units;
	glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &max_combined_texture_image_units);
	// CUSTOM_TRACE("texture units available: %d", max_combined_texture_image_units);
	ogl.unit_ids.set_capacity(max_combined_texture_image_units);
	for (GLint i = 0; i < max_combined_texture_image_units; ++i) {
		ogl.unit_ids.get(i) = {empty_asset_id, empty_asset_id};
	}
	// {
	// 	GLint value;
	// 	glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, &value);
	// 	CUSTOM_TRACE("  - vertex ....... %d", value);
	// 	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &value);
	// 	CUSTOM_TRACE("  - fragment ..... %d", value);
	// 	glGetIntegerv(GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS, &value);
	// 	CUSTOM_TRACE("  - geometry ..... %d", value);
	// 	glGetIntegerv(GL_MAX_COMPUTE_TEXTURE_IMAGE_UNITS, &value);
	// 	CUSTOM_TRACE("  - compute ...... %d", value);
	// 	glGetIntegerv(GL_MAX_TESS_CONTROL_TEXTURE_IMAGE_UNITS, &value);
	// 	CUSTOM_TRACE("  - tess control . %d", value);
	// 	glGetIntegerv(GL_MAX_TESS_EVALUATION_TEXTURE_IMAGE_UNITS, &value);
	// 	CUSTOM_TRACE("  - tess eval .... %d", value);
	// }

	GLint max_samples;
	glGetIntegerv(GL_MAX_SAMPLES, &max_samples);
}

void shutdown(void) {
	ogl.opengl::Data::~Data();
}

#define INSTRUCTION_IMPL(T) static void platform_##T(Bytecode const & bc);
#include "engine/api/instructions_registry_impl.h"

void consume(Bytecode const & bc) {
	while (bc.offset < bc.buffer.count) {
		Instruction instruction = *bc.read<Instruction>();

		switch (instruction)
		{
			case Instruction::None: CUSTOM_ASSERT(false, "null instruction encountered"); break;
			#define INSTRUCTION_IMPL(T) case Instruction::T: platform_##T(bc); break;
			#include "engine/api/instructions_registry_impl.h"
			case Instruction::Last: CUSTOM_ASSERT(false, "non-instruction encountered"); break;
			default: CUSTOM_ASSERT(false, "unknown instruction encountered: %d", instruction); break;
		}
		PLATFORM_CONSUME_ERRORS();
	}
}

//

bool is_inited_uniforms() {
	return ogl.uniform_names.count > 0;
}

bool is_allocated_shader(u32 id) {
	if (id >= ogl.programs.capacity) { return false; }
	return ogl.programs.get(id).id != empty_gl_id;
}

bool is_allocated_texture(u32 id) {
	if (id >= ogl.textures.capacity) { return false; }
	return ogl.textures.get(id).id != empty_gl_id;
}

bool is_allocated_sampler(u32 id) {
	if (id >= ogl.samplers.capacity) { return false; }
	return ogl.samplers.get(id).id != empty_gl_id;
}

bool is_allocated_mesh(u32 id) {
	if (id >= ogl.meshes.capacity) { return false; }
	return ogl.meshes.get(id).id != empty_gl_id;
}

bool is_allocated_target(u32 id) {
	if (id >= ogl.targets.capacity) { return false; }
	return ogl.targets.get(id).id != empty_gl_id;
}

//

bool is_uploaded_shader(u32 id) {
	return ogl.programs.get(id).uploaded;
}

bool is_uploaded_texture(u32 id) {
	return ogl.textures.get(id).uploaded;
}

bool is_uploaded_mesh(u32 id) {
	return ogl.meshes.get(id).uploaded;
}

}}

//
// platform implementation
//

#if !defined(PLATFORM_INIT_DEBUG)
static void opengl_message_callback(
	GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	glstring message,
	cmemory userParam)
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
		case GL_DEBUG_SEVERITY_HIGH:         severity_string = ANSI_BKG_RED "[crt]" ANSI_CLR; break; // All OpenGL Errors, shader compilation/linking errors, or highly-dangerous undefined behavior
		case GL_DEBUG_SEVERITY_MEDIUM:       severity_string = ANSI_TXT_RED "[err]" ANSI_CLR; break; // Major performance warnings, shader compilation/linking warnings, or the use of deprecated functionality
		case GL_DEBUG_SEVERITY_LOW:          severity_string = ANSI_TXT_YLW "[wrn]" ANSI_CLR; break; // Redundant state change performance warning, or unimportant undefined behavior
		case GL_DEBUG_SEVERITY_NOTIFICATION: severity_string = ANSI_TXT_GRY "[trc]" ANSI_CLR; break; // Anything that isn't an error or performance issue.
		default:                             severity_string = ANSI_BKG_MGT "[???]" ANSI_CLR; break; // ?
	}

	CUSTOM_TRACE(
		"OpenGL message '0x%x'"
		ANSI_TXT_GRY "\n  - message:  " ANSI_CLR "%s"
		ANSI_TXT_GRY "\n  - severity: " ANSI_CLR "%s"
		ANSI_TXT_GRY "\n  - type:     " ANSI_CLR "%s"
		ANSI_TXT_GRY "\n  - source:   " ANSI_CLR "%s",
		id,
		message,
		severity_string,
		type_string,
		source_string
	);
}

static void PLATFORM_INIT_DEBUG()
{
	if (ogl.version >= COMPILE_VERSION(4, 3)) {
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(opengl_message_callback, nullptr);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_FALSE);
		CUSTOM_TRACE("enabled OpenGL debug");
	}
	else {
		CUSTOM_WARNING("OpenGL debug is unavailable");
	}
}
#endif // !defined(PLATFORM_INIT_DEBUG)

#if !defined(PLATFORM_CONSUME_ERRORS)
#define CASE_IMPL(T) case T: CUSTOM_ASSERT(false, "OpenGL error '0x%x': " #T, error); break
static void PLATFORM_CONSUME_ERRORS()
{
	GLenum error;
	while ((error = glGetError()) != GL_NO_ERROR) {
		switch (error) {
			CASE_IMPL(GL_INVALID_ENUM);
			CASE_IMPL(GL_INVALID_VALUE);
			CASE_IMPL(GL_INVALID_OPERATION);
			CASE_IMPL(GL_INVALID_FRAMEBUFFER_OPERATION);
			CASE_IMPL(GL_OUT_OF_MEMORY);
			CASE_IMPL(GL_STACK_UNDERFLOW);
			CASE_IMPL(GL_STACK_OVERFLOW);
			default: CUSTOM_ASSERT(false, "OpenGL error '0x%x': unknown", error);
		}
	}
}
#undef CASE_IMPL
#endif // !defined(PLATFORM_CONSUME_ERRORS)

// static void platform_get_shader_source(GLuint id, custom::Array<GLchar> & buffer)
// {
// 	GLint max_length;
// 	glGetShaderiv(id, GL_SHADER_SOURCE_LENGTH, &max_length);
// 	if (max_length) {
// 		buffer.set_capacity(max_length);
// 		glGetShaderSource(id, max_length, &max_length, buffer.data);
// 		buffer.count = max_length;
// 	}
// }

// static void platform_print_shader_source(GLuint id)
// {
// 	custom::Array<GLchar> text;
// 	platform_get_shader_source(id, text);
// 	if (text.count) {
// 		// for (u32 i = 0; i < text.count; ++i) { if (text[i] == '\t') { text[i] = ' '; } }
// 		CUSTOM_TRACE("shader source:\n%s", text.data);
// 	}
// 	else {
// 		CUSTOM_TRACE("shader source: unknown");
// 	}
// }

// static void platform_get_program_binary(GLuint id, GLenum & format, custom::Array<u8> & buffer)
// {
// 	GLint max_length;
// 	glGetProgramiv(id, GL_PROGRAM_BINARY_LENGTH, &max_length);
// 	if (max_length) {
// 		buffer.set_capacity(max_length);
// 		glGetProgramBinary(id, max_length, &max_length, &format, buffer.data);
// 		buffer.count = max_length;
// 	}
// }

static bool verify_shader(GLuint id, GLenum parameter)
{
	// GL_DELETE_STATUS
	// GL_COMPILE_STATUS

	GLint status;
	glGetShaderiv(id, parameter, &status);
	if (status) { return true; }

	// @Note: linker will inform of the errors anyway
	GLint max_length;
	glGetShaderiv(id, GL_INFO_LOG_LENGTH, &max_length);

	if (max_length) {
		custom::Array<GLchar> text(max_length);
		glGetShaderInfoLog(id, max_length, &max_length, text.data);
		CUSTOM_TRACE("shader status:\n%s", text.data);
	}
	
	// platform_print_shader_source(id);

	return false;
}

static bool platform_verify_program(GLuint id, GLenum parameter)
{
	// GL_DELETE_STATUS
	// GL_LINK_STATUS
	// GL_VALIDATE_STATUS

	GLint status;
	glGetProgramiv(id, parameter, &status);
	if (status) { return true; }

	GLint max_length;
	glGetProgramiv(id, GL_INFO_LOG_LENGTH, &max_length);

	if (max_length) {
		custom::Array<GLchar> text(max_length);
		glGetProgramInfoLog(id, max_length, &max_length, text.data);
		CUSTOM_TRACE("program status:\n%s", text.data);
	}

	return false;
}

struct GL_String { GLint count; glstring data; };
struct Shader_Props
{
	GLenum    type;
	GL_String version;
	GL_String defines;
};

static u8 fill_props(custom::graphics::Shader_Part parts, Shader_Props * props, u8 cap)
{
	u8 count = 0;

	if (bits_are_set(parts, custom::graphics::Shader_Part::Vertex)) {
		constexpr static GLchar const version[] = "#version 330 core\n";
		constexpr static GLchar const defines[] = "#define VERTEX_SECTION";
		props[count++] = { GL_VERTEX_SHADER, {C_ARRAY_LENGTH(version), version}, {C_ARRAY_LENGTH(defines), defines} };
	}

	if (bits_are_set(parts, custom::graphics::Shader_Part::Pixel)) {
		constexpr static GLchar const version[] = "#version 330 core\n";
		constexpr static GLchar const defines[] = "#define FRAGMENT_SECTION\n";
		props[count++] = { GL_FRAGMENT_SHADER, {C_ARRAY_LENGTH(version), version}, {C_ARRAY_LENGTH(defines), defines} };
	}

	if (bits_are_set(parts, custom::graphics::Shader_Part::Geometry)) {
		constexpr static GLchar const version[] = "#version 330 core\n";
		constexpr static GLchar const defines[] = "#define GEOMETRY_SECTION\n";
		props[count++] = { GL_GEOMETRY_SHADER, {C_ARRAY_LENGTH(version), version}, {C_ARRAY_LENGTH(defines), defines} };
	}

	if (bits_are_set(parts, custom::graphics::Shader_Part::Compute)) {
		constexpr static GLchar const version[] = "#version 430 core\n";
		constexpr static GLchar const defines[] = "#define COMPUTE_SECTION\n";
		props[count++] = { GL_COMPUTE_SHADER, {C_ARRAY_LENGTH(version), version}, {C_ARRAY_LENGTH(defines), defines} };
	}

	// @Note: exclude '\0' from the length
	//        alternative is to call `strlen` instead of `C_ARRAY_LENGTH`
	for (u8 i = 0; i < count; ++i) {
		--props[i].version.count;
		--props[i].defines.count;
	}

	return count;
}

static bool platform_link_program(GLuint program_id, GL_String source, custom::graphics::Shader_Part parts)
{
	u8 const props_cap = 4;
	Shader_Props props[props_cap];
	GLuint       shaders[props_cap];
	u8 props_count = fill_props(parts, props, props_cap);

	// Compile shaders
	for (u8 i = 0; i < props_count; ++i) {
		glstring code[] = { props[i].version.data,  props[i].defines.data,  source.data };
		GLint length[]  = { props[i].version.count, props[i].defines.count, source.count };
		GLuint shader_id = glCreateShader(props[i].type);
		glShaderSource(shader_id, C_ARRAY_LENGTH(code), code, length);
		glCompileShader(shader_id);
		shaders[i] = shader_id;
	}

	bool is_compiled = true;
	for (u8 i = 0; i < props_count; ++i) {
		bool isOk = verify_shader(shaders[i], GL_COMPILE_STATUS);
		is_compiled = is_compiled && isOk;
	}

	// Link the program
	for (u8 i = 0; i < props_count; ++i) {
		glAttachShader(program_id, shaders[i]);
	}
	glLinkProgram(program_id);
	bool is_linked = platform_verify_program(program_id, GL_LINK_STATUS);

	// Free shader resources
	for (u8 i = 0; i < props_count; ++i) {
		glDetachShader(program_id, shaders[i]);
	}
	for (u8 i = 0; i < props_count; ++i) {
		glDeleteShader(shaders[i]);
	}

	return is_compiled && is_linked;
}

// static GLenum get_basic_type(GLenum value) {
// 	switch (value) {
// 		case GL_SAMPLER_2D: return GL_INT;
// 		//
// 		case GL_FLOAT:      return GL_FLOAT;
// 		case GL_FLOAT_VEC2: return GL_FLOAT;
// 		case GL_FLOAT_VEC3: return GL_FLOAT;
// 		case GL_FLOAT_VEC4: return GL_FLOAT;
// 		//
// 		case GL_INT:      return GL_INT;
// 		case GL_INT_VEC2: return GL_INT;
// 		case GL_INT_VEC3: return GL_INT;
// 		case GL_INT_VEC4: return GL_INT;
// 		//
// 		case GL_UNSIGNED_INT:      return GL_UNSIGNED_INT;
// 		case GL_UNSIGNED_INT_VEC2: return GL_UNSIGNED_INT;
// 		case GL_UNSIGNED_INT_VEC3: return GL_UNSIGNED_INT;
// 		case GL_UNSIGNED_INT_VEC4: return GL_UNSIGNED_INT;
// 		//
// 		case GL_FLOAT_MAT2: return GL_FLOAT;
// 		case GL_FLOAT_MAT3: return GL_FLOAT;
// 		case GL_FLOAT_MAT4: return GL_FLOAT;
// 	}
// 	CUSTOM_WARNING("unknown OGL type 0x%x", value);
// 	return GL_NONE;
// }

// static void platform_get_active_attribute(GLuint id, GLuint index, Program_Field & buffer)
// {
// 	glGetActiveAttrib(
// 		id, index,
// 		sizeof(buffer.name), &buffer.name_count,
// 		&buffer.size, &buffer.type, buffer.name
// 	);
// 	buffer.location = glGetAttribLocation(id, buffer.name);
// }

static void platform_get_active_uniform(GLuint id, GLuint index, Program_Field & buffer)
{
	glGetActiveUniform(
		id, index,
		sizeof(buffer.name), &buffer.name_count,
		&buffer.size, &buffer.type, buffer.name
	);
	buffer.location = glGetUniformLocation(id, buffer.name);
}

//
//
//

namespace custom {
namespace graphics {

static GLenum get_comparison(Comparison value) {
	switch (value) {
		case Comparison::False:   return GL_NEVER;
		case Comparison::Less:    return GL_LESS;
		case Comparison::LEqual:  return GL_LEQUAL;
		case Comparison::Equal:   return GL_EQUAL;
		case Comparison::NEqual:  return GL_NOTEQUAL;
		case Comparison::GEqual:  return GL_GEQUAL;
		case Comparison::Greater: return GL_GREATER;
		case Comparison::True:    return GL_ALWAYS;
	}
	CUSTOM_ASSERT(false, "unknown comparison %d", value);
	return GL_NONE;
}

static GLenum get_clip_origin(Clip_Origin value) {
	switch (value) {
		case Clip_Origin::Lower_Left: return GL_LOWER_LEFT;
		case Clip_Origin::Upper_Left: return GL_UPPER_LEFT;
	}
	CUSTOM_ASSERT(false, "unknown clip origin %d", value);
	return GL_NONE;
}

static GLenum get_clip_depth(Clip_Depth value) {
	switch (value) {
		case Clip_Depth::NegOne_One: return GL_NEGATIVE_ONE_TO_ONE;
		case Clip_Depth::Zero_One: return GL_ZERO_TO_ONE;
	}
	CUSTOM_ASSERT(false, "unknown clip depth %d", value);
	return GL_NONE;
}

static GLenum get_cull_mode(Cull_Mode value) {
	switch (value) {
		case Cull_Mode::Back:  return GL_BACK;
		case Cull_Mode::Front: return GL_FRONT;
		case Cull_Mode::Both:  return GL_FRONT_AND_BACK;
	}
	CUSTOM_ASSERT(false, "unknown cull mode %d", value);
	return GL_NONE;
}

static GLenum get_front_face(Front_Face value) {
	switch (value) {
		case Front_Face::CW:  return GL_CW;
		case Front_Face::CCW: return GL_CCW;
	}
	CUSTOM_ASSERT(false, "unknown front face %d", value);
	return GL_NONE;
}

static GLenum get_operation(Operation value) {
	switch (value) {
		case Operation::Keep:      return GL_KEEP;
		case Operation::Zero:      return GL_ZERO;
		case Operation::Replace:   return GL_REPLACE;
		case Operation::Incr:      return GL_INCR;
		case Operation::Incr_Wrap: return GL_INCR_WRAP;
		case Operation::Decr:      return GL_DECR;
		case Operation::Decr_Wrap: return GL_DECR_WRAP;
		case Operation::Invert:    return GL_INVERT;
	}
	CUSTOM_ASSERT(false, "unknown operation %d", value);
	return GL_NONE;
}

static GLenum get_texture_internal_format(Texture_Type texture_type, Data_Type data_type, u8 channels) {
	switch (texture_type) {
		case Texture_Type::Color: switch (data_type) {
			case Data_Type::u8: switch (channels) {
				case 1: return GL_R8;
				case 2: return GL_RG8;
				case 3: return GL_RGB8;
				case 4: return GL_RGBA8;
			} break;
			case Data_Type::u16: switch (channels) {
				case 1: return GL_R16;
				case 2: return GL_RG16;
				case 3: return GL_RGB16;
				case 4: return GL_RGBA16;
			} break;
			case Data_Type::u32: switch (channels) {
				case 1: return GL_R32UI;
				case 2: return GL_RG32UI;
				case 3: return GL_RGB32UI;
				case 4: return GL_RGBA32UI;
			} break;
			case Data_Type::r32: switch (channels) {
				case 1: return GL_R32F;
				case 2: return GL_RG32F;
				case 3: return GL_RGB32F;
				case 4: return GL_RGBA32F;
			} break;
		} break;

		case Texture_Type::Depth: switch (data_type) {
			case Data_Type::u16: return GL_DEPTH_COMPONENT16;
			case Data_Type::u32: return GL_DEPTH_COMPONENT24;
			case Data_Type::r32: return GL_DEPTH_COMPONENT32F;
		} break;

		case Texture_Type::DStencil: switch (data_type) {
			case Data_Type::u32: return GL_DEPTH24_STENCIL8;
			case Data_Type::r32: return GL_DEPTH32F_STENCIL8;
		}

		case Texture_Type::Stencil: switch (data_type) {
			case Data_Type::u8: return GL_STENCIL_INDEX8;
		}
	}
	CUSTOM_ASSERT(false, "unknown texture type %d with data type %d and channels count %d", texture_type, data_type, channels);
	return GL_NONE;
}

static GLenum get_texture_data_format(Texture_Type texture_type, u8 channels) {
	switch (texture_type) {
		case Texture_Type::Color: switch (channels) {
			case 1: return GL_RED;
			case 2: return GL_RG;
			case 3: return GL_RGB;
			case 4: return GL_RGBA;
		} break;

		case Texture_Type::Depth:
			return GL_DEPTH_COMPONENT;

		case Texture_Type::DStencil:
			return GL_DEPTH_STENCIL;

		case Texture_Type::Stencil:
			return GL_STENCIL_INDEX;
	}
	CUSTOM_ASSERT(false, "unknown texture type %d and channels count %d", texture_type, channels);
	return GL_NONE;
}

static GLenum get_attachment_format(Texture_Type texture_type, u8 index) {
	switch (texture_type) {
		case Texture_Type::Color:
			return GL_COLOR_ATTACHMENT0 + index;

		case Texture_Type::Depth:
			return GL_DEPTH_ATTACHMENT;

		case Texture_Type::DStencil:
			return GL_DEPTH_STENCIL_ATTACHMENT;

		case Texture_Type::Stencil:
			return GL_STENCIL_ATTACHMENT;
	}
	CUSTOM_ASSERT(false, "unknown texture type %d and index %d", texture_type, index);
	return GL_NONE;
}

static GLenum get_min_filter(Filter_Mode texture_filter, Filter_Mode mipmap_filter) {
	switch (mipmap_filter) {
		case Filter_Mode::None: switch (texture_filter) {
			case Filter_Mode::None:   return GL_NEAREST;
			case Filter_Mode::Point:  return GL_NEAREST;
			case Filter_Mode::Linear: return GL_LINEAR;
		} break;
		case Filter_Mode::Point: switch (texture_filter) {
			case Filter_Mode::None:   return GL_NEAREST_MIPMAP_NEAREST;
			case Filter_Mode::Point:  return GL_NEAREST_MIPMAP_NEAREST;
			case Filter_Mode::Linear: return GL_LINEAR_MIPMAP_NEAREST;
		} break;
		case Filter_Mode::Linear: switch (texture_filter) {
			case Filter_Mode::None:   return GL_LINEAR_MIPMAP_NEAREST;
			case Filter_Mode::Point:  return GL_LINEAR_MIPMAP_NEAREST;
			case Filter_Mode::Linear: return GL_LINEAR_MIPMAP_LINEAR;
		} break;
	}
	CUSTOM_ASSERT(false, "unknown texture filter %d and mipmap filter %d", texture_filter, mipmap_filter);
	return GL_NONE;
}

static GLenum get_mag_filter(Filter_Mode value) {
	switch (value) {
		case Filter_Mode::None:   return GL_NEAREST;
		case Filter_Mode::Point:  return GL_NEAREST;
		case Filter_Mode::Linear: return GL_LINEAR;
	}
	CUSTOM_ASSERT(false, "unknown filter mode %d", value);
	return GL_NONE;
}

static GLenum get_wrap_mode(Wrap_Mode value) {
	switch (value) {
		case Wrap_Mode::Repeat: return GL_REPEAT;
		case Wrap_Mode::Clamp:  return GL_CLAMP_TO_EDGE;
		case Wrap_Mode::Mirror_Repeat: return GL_MIRRORED_REPEAT;
		case Wrap_Mode::Mirror_Clamp:  return GL_MIRROR_CLAMP_TO_EDGE;
	}
	CUSTOM_ASSERT(false, "unknown wrap mode %d", value);
	return GL_NONE;
}

static GLenum get_texture_data_type(Texture_Type texture_type, Data_Type data_type) {
	switch (texture_type) {
		case Texture_Type::Color: switch (data_type) {
			case Data_Type::u8:  return GL_UNSIGNED_BYTE;
			case Data_Type::u16: return GL_UNSIGNED_SHORT;
			case Data_Type::u32: return GL_UNSIGNED_INT;
			case Data_Type::r32: return GL_FLOAT;
		} break;

		case Texture_Type::Depth: switch (data_type) {
			case Data_Type::u16: return GL_UNSIGNED_SHORT;
			case Data_Type::u32: return GL_UNSIGNED_INT;
			case Data_Type::r32: return GL_FLOAT;
		} break;

		case Texture_Type::DStencil: switch (data_type) {
			case Data_Type::u32: return GL_UNSIGNED_INT_24_8;
			case Data_Type::r32: return GL_FLOAT_32_UNSIGNED_INT_24_8_REV;
		}

		case Texture_Type::Stencil: switch (data_type) {
			case Data_Type::u8: return GL_UNSIGNED_BYTE;
		}
	}
	CUSTOM_ASSERT(false, "unknown texture type %d with data type %d", texture_type, data_type);
	return GL_NONE;
}

static GLenum get_data_type(Data_Type value) {
	switch (value) {
		case Data_Type::unit_id: return GL_INT;
		//
		case Data_Type::s8:  return GL_BYTE;
		case Data_Type::s16: return GL_SHORT;
		case Data_Type::s32: return GL_INT;
		//
		case Data_Type::u8:  return GL_UNSIGNED_BYTE;
		case Data_Type::u16: return GL_UNSIGNED_SHORT;
		case Data_Type::u32: return GL_UNSIGNED_INT;
		//
		case Data_Type::r32: return GL_FLOAT;
		case Data_Type::r64: return GL_DOUBLE;
		//
		case Data_Type::vec2: return GL_FLOAT;
		case Data_Type::vec3: return GL_FLOAT;
		case Data_Type::vec4: return GL_FLOAT;
		//
		case Data_Type::ivec2: return GL_INT;
		case Data_Type::ivec3: return GL_INT;
		case Data_Type::ivec4: return GL_INT;
		//
		case Data_Type::uvec2: return GL_UNSIGNED_INT;
		case Data_Type::uvec3: return GL_UNSIGNED_INT;
		case Data_Type::uvec4: return GL_UNSIGNED_INT;
		//
		case Data_Type::mat2: return GL_FLOAT;
		case Data_Type::mat3: return GL_FLOAT;
		case Data_Type::mat4: return GL_FLOAT;
	};
	CUSTOM_ASSERT(false, "unknown data type %d", value);
	return GL_NONE;
}

#define CASE_IMPL(T) case Data_Type::T: return sizeof(T)
static u16 get_type_size(Data_Type value) {
	switch (value) {
		CASE_IMPL(unit_id);
		CASE_IMPL(s8); CASE_IMPL(s16); CASE_IMPL(s32);
		CASE_IMPL(u8); CASE_IMPL(u16); CASE_IMPL(u32);
		CASE_IMPL(r32); CASE_IMPL(r64);
		CASE_IMPL(vec2); CASE_IMPL(vec3); CASE_IMPL(vec4);
		CASE_IMPL(ivec2); CASE_IMPL(ivec3); CASE_IMPL(ivec4);
		CASE_IMPL(uvec2); CASE_IMPL(uvec3); CASE_IMPL(uvec4);
		CASE_IMPL(mat2); CASE_IMPL(mat3); CASE_IMPL(mat4);
	}
	CUSTOM_ASSERT(false, "unknown data type %d", value);
	return 0;
}
#undef CASE_IMPL

#define CASE_IMPL(T) case Data_Type::T: return bc.read<T>(count)
static cmemory read_data(Bytecode const & bc, Data_Type type, u32 count) {
	switch (type) {
		CASE_IMPL(unit_id);
		CASE_IMPL(s8); CASE_IMPL(s16); CASE_IMPL(s32);
		CASE_IMPL(u8); CASE_IMPL(u16); CASE_IMPL(u32);
		CASE_IMPL(r32); CASE_IMPL(r64);
		CASE_IMPL(vec2); CASE_IMPL(vec3); CASE_IMPL(vec4);
		CASE_IMPL(ivec2); CASE_IMPL(ivec3); CASE_IMPL(ivec4);
		CASE_IMPL(uvec2); CASE_IMPL(uvec3); CASE_IMPL(uvec4);
		CASE_IMPL(mat2); CASE_IMPL(mat3); CASE_IMPL(mat4);
	}
	CUSTOM_ASSERT(false, "unknown data type %d", type);
	return NULL;
}
#undef CASE_IMPL

struct DT_Array { Data_Type type; u32 count; cmemory data; };
static DT_Array read_data_array(Bytecode const & bc) {
	Data_Type type  = *bc.read<Data_Type>();
	u32       count = *bc.read<u32>();
	cmemory   data  = read_data(bc, type, count);
	return { type, count, data };
}

struct Inline_String { u32 count; cstring data; };
static Inline_String read_cstring(Bytecode const & bc) {
	u32 count = *bc.read<u32>();
	cstring data = bc.read<char>(count);
	return { count, data };
}

static GLenum get_mesh_usage(Mesh_Frequency frequency, Mesh_Access access) {
	switch (frequency) {
		case Mesh_Frequency::Static: switch (access) {
			case Mesh_Access::Draw: return GL_STATIC_DRAW;
			case Mesh_Access::Read: return GL_STATIC_READ;
			case Mesh_Access::Copy: return GL_STATIC_COPY;
		}
		case Mesh_Frequency::Dynamic: switch (access) {
			case Mesh_Access::Draw: return GL_DYNAMIC_DRAW;
			case Mesh_Access::Read: return GL_DYNAMIC_READ;
			case Mesh_Access::Copy: return GL_DYNAMIC_COPY;
		}
		case Mesh_Frequency::Stream: switch (access) {
			case Mesh_Access::Draw: return GL_STREAM_DRAW;
			case Mesh_Access::Read: return GL_STREAM_READ;
			case Mesh_Access::Copy: return GL_STREAM_COPY;
		}
	}
	CUSTOM_ASSERT(false, "unknown frequency %d and access %d", frequency, access);
	return GL_NONE;
}

}}

//
//
//

namespace custom {
namespace graphics {

static void platform_Depth_Read(Bytecode const & bc) {
	b8 value = *bc.read<b8>();
	if (value) {
		glEnable(GL_DEPTH_TEST);
	}
	else {
		glDisable(GL_DEPTH_TEST);
	}
}

static void platform_Depth_Write(Bytecode const & bc) {
	b8 value = *bc.read<b8>();
	glDepthMask(value);
}

static void platform_Depth_Range(Bytecode const & bc) {
	vec2 value = *bc.read<vec2>();
	if (ogl.version >= COMPILE_VERSION(4, 1)) {
		glDepthRangef(value.x, value.y);
	}
	else {
		glDepthRange(value.x, value.y);
	}
}

static void platform_Depth_Comparison(Bytecode const & bc) {
	Comparison value = *bc.read<Comparison>();
	glDepthFunc(get_comparison(value));
}

static void platform_Depth_Clear(Bytecode const & bc) {
	r32 value = *bc.read<r32>();
	if (ogl.version >= COMPILE_VERSION(4, 1)) {
		glClearDepthf(value);
	}
	else {
		glClearDepth(value);
	}
}

static void platform_Color_Write(Bytecode const & bc) {
	Color_Write value = *bc.read<Color_Write>();
	glColorMask(
		bits_are_set(value, Color_Write::R),
		bits_are_set(value, Color_Write::G),
		bits_are_set(value, Color_Write::B),
		bits_are_set(value, Color_Write::A)
	);
}

static void platform_Color_Clear(Bytecode const & bc) {
	vec4 value = *bc.read<vec4>();
	glClearColor(value.x, value.y, value.z, value.w);
}

static void platform_Blend_Mode(Bytecode const & bc) {
	Blend_Mode value = *bc.read<Blend_Mode>();

	if (value == Blend_Mode::Opaque) {
		glDisable(GL_BLEND);
		return;
	}

	glEnable(GL_BLEND);
	switch (value) {
		case Blend_Mode::Alpha:
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			// glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA); // premultiply alpha
			return;
		case Blend_Mode::Additive:
			glBlendFunc(GL_SRC_ALPHA, GL_ONE);
			// glBlendFunc(GL_ONE, GL_ONE); // premultiply additive
			return;
		case Blend_Mode::Multiply:
			glBlendFunc(GL_DST_COLOR, GL_ZERO);
			// glBlendFunc(GL_ZERO, GL_SRC_COLOR); // just the same
			return;
	}
	CUSTOM_ASSERT(false, "unknown blend mode %d", value);
}

static void platform_Cull_Mode(Bytecode const & bc) {
	Cull_Mode value = *bc.read<Cull_Mode>();

	if (value == Cull_Mode::None) {
		glDisable(GL_CULL_FACE);
		return;
	}

	glEnable(GL_CULL_FACE);
	glCullFace(get_cull_mode(value));
}

static void platform_Front_Face(Bytecode const & bc) {
	Front_Face value = *bc.read<Front_Face>();
	glFrontFace(get_front_face(value));
}

static void platform_Clip_Control(Bytecode const & bc) {
	Clip_Origin origin = *bc.read<Clip_Origin>();
	Clip_Depth depth = *bc.read<Clip_Depth>();

	if (ogl.version >= COMPILE_VERSION(4, 5)) {
		glClipControl(get_clip_origin(origin), get_clip_depth(depth));
		glClipControl(GL_LOWER_LEFT, GL_ZERO_TO_ONE);
	}
	else {
		CUSTOM_WARNING("no clip control");
	}
}

static void platform_Stencil_Read(Bytecode const & bc) {
	b8 value = *bc.read<b8>();
	if (value) {
		glEnable(GL_STENCIL_TEST);
	}
	else {
		glDisable(GL_STENCIL_TEST);
	}
}

static void platform_Stencil_Write(Bytecode const & bc) {
	u32 value = *bc.read<u32>();
	glStencilMask(value);
}

static void platform_Stencil_Comparison(Bytecode const & bc) {
	Comparison value = *bc.read<Comparison>();
	u8 ref  = *bc.read<u8>();
	u8 mask = *bc.read<u8>();
	glStencilFunc(get_comparison(value), ref, mask);
}

static void platform_Stencil_Operation(Bytecode const & bc) {
	// stencil test + depth test
	Operation fail_any  = *bc.read<Operation>();
	Operation succ_fail = *bc.read<Operation>();
	Operation succ_succ = *bc.read<Operation>();
	glStencilOp(
		get_operation(fail_any),
		get_operation(succ_fail),
		get_operation(succ_succ)
	);
}

static void platform_Stencil_Mask(Bytecode const & bc) {
	u8 mask = *bc.read<u8>();
	glStencilMask(mask);
}

static void platform_Allocate_Shader(Bytecode const & bc) {
	u32     asset_id = *bc.read<u32>();
	ogl.programs_ensure_capacity(asset_id);
	opengl::Program * resource = &ogl.programs.get(asset_id);
	if (resource->id != empty_gl_id) {
		CUSTOM_WARNING("shader %d already exists", asset_id);
		return;
	}
	new (resource) opengl::Program;
	resource->id = glCreateProgram();
}

static void platform_consume_texture_params(Bytecode const & bc, opengl::Texture * resource) {
	new (resource) opengl::Texture;
	resource->target       = GL_TEXTURE_2D;
	resource->is_dynamic   = *bc.read<b8>();
	resource->size         = *bc.read<ivec2>();
	resource->channels     = *bc.read<u8>();
	resource->data_type    = *bc.read<Data_Type>();
	resource->texture_type = *bc.read<Texture_Type>();
	resource->min_tex      = *bc.read<Filter_Mode>();
	resource->min_mip      = *bc.read<Filter_Mode>();
	resource->mag_tex      = *bc.read<Filter_Mode>();
	resource->wrap_x       = *bc.read<Wrap_Mode>();
	resource->wrap_y       = *bc.read<Wrap_Mode>();
}

static void platform_Allocate_Texture(Bytecode const & bc) {
	u32   asset_id = *bc.read<u32>();
	ogl.textures_ensure_capacity(asset_id);
	opengl::Texture * resource = &ogl.textures.get(asset_id);
	if (resource->id != empty_gl_id) {
		CUSTOM_WARNING("texture %d already exists", asset_id);
		opengl::Texture default_texture;
		platform_consume_texture_params(bc, &default_texture);
		return;
	}
	platform_consume_texture_params(bc, resource);

	// -- allocate memory --
	if (ogl.version >= COMPILE_VERSION(4, 5)) {
		glCreateTextures(resource->target, 1, &resource->id);
		glTextureStorage2D(
			resource->id, 1,
			get_texture_internal_format(resource->texture_type, resource->data_type, resource->channels),
			resource->size.x, resource->size.y
		);
	}
	else {
		glGenTextures(1, &resource->id);
		glBindTexture(resource->target, resource->id);
		if (ogl.version >= COMPILE_VERSION(4, 2)) {
			glTexStorage2D(
				resource->target, 1,
				get_texture_internal_format(resource->texture_type, resource->data_type, resource->channels),
				resource->size.x, resource->size.y
			);
		}
		else {
			glTexImage2D(
				resource->target, 0,
				get_texture_internal_format(resource->texture_type, resource->data_type, resource->channels),
				resource->size.x, resource->size.y, 0,
				get_texture_data_format(resource->texture_type, resource->channels),
				get_texture_data_type(resource->texture_type, resource->data_type),
				NULL
			);
		}
	}

	// -- chart memory --
	if (ogl.version >= COMPILE_VERSION(4, 5)) {
		glTextureParameteri(resource->id, GL_TEXTURE_MIN_FILTER, get_min_filter(resource->min_tex, resource->min_mip));
		glTextureParameteri(resource->id, GL_TEXTURE_MAG_FILTER, get_mag_filter(resource->mag_tex));
		glTextureParameteri(resource->id, GL_TEXTURE_WRAP_S, get_wrap_mode(resource->wrap_x));
		glTextureParameteri(resource->id, GL_TEXTURE_WRAP_T, get_wrap_mode(resource->wrap_y));
	}
	else {
		// glBindTexture(target, resource->id);
		glTexParameteri(resource->target, GL_TEXTURE_MIN_FILTER, get_min_filter(resource->min_tex, resource->min_mip));
		glTexParameteri(resource->target, GL_TEXTURE_MAG_FILTER, get_mag_filter(resource->mag_tex));
		glTexParameteri(resource->target, GL_TEXTURE_WRAP_S, get_wrap_mode(resource->wrap_x));
		glTexParameteri(resource->target, GL_TEXTURE_WRAP_T, get_wrap_mode(resource->wrap_y));
	}
}

static void platform_Allocate_Sampler(Bytecode const & bc) {
	u32   asset_id = *bc.read<u32>();
	ogl.samplers_ensure_capacity(asset_id);
	opengl::Sampler * resource = &ogl.samplers.get(asset_id);
	if (resource->id != empty_gl_id) { CUSTOM_WARNING("sampler %d already exists", asset_id); return; }
	new (resource) opengl::Sampler;

	resource->min_tex = *bc.read<Filter_Mode>();
	resource->min_mip = *bc.read<Filter_Mode>();
	resource->mag_tex = *bc.read<Filter_Mode>();
	resource->wrap_x  = *bc.read<Wrap_Mode>();
	resource->wrap_y  = *bc.read<Wrap_Mode>();

	CUSTOM_ASSERT(ogl.version >= COMPILE_VERSION(3, 2), "samplers are not supported");
	if (ogl.version >= COMPILE_VERSION(4, 5)) {
		glCreateSamplers(1, &resource->id);
	}
	else {
		glGenSamplers(1, &resource->id);
	}

	glSamplerParameteri(resource->id, GL_TEXTURE_MIN_FILTER, get_min_filter(resource->min_tex, resource->min_mip));
	glSamplerParameteri(resource->id, GL_TEXTURE_MAG_FILTER, get_mag_filter(resource->mag_tex));
	glSamplerParameteri(resource->id, GL_TEXTURE_WRAP_S, get_wrap_mode(resource->wrap_x));
	glSamplerParameteri(resource->id, GL_TEXTURE_WRAP_T, get_wrap_mode(resource->wrap_y));
}

static void platform_consume_mesh_params(Bytecode const & bc, opengl::Mesh * resource) {
	new (resource) opengl::Mesh;
	u8 buffers_count = *bc.read<u8>();
	// resource->buffers.set_capacity(buffers_count);
	CUSTOM_ASSERT(buffers_count <= resource->buffers.capacity, "too many buffers");
	for (u8 i = 0; i < buffers_count; ++i) {
		resource->buffers.push();
		opengl::Buffer * buffer = new (&resource->buffers[i]) opengl::Buffer;

		buffer->is_index  = *bc.read<b8>();
		buffer->frequency = *bc.read<Mesh_Frequency>();
		buffer->access    = *bc.read<Mesh_Access>();
		buffer->type      = *bc.read<Data_Type>();
		buffer->capacity  = *bc.read<u32>();
		buffer->count     = *bc.read<u32>();

		u32 attr_count = *bc.read<u32>();
		// buffer->attributes.set_capacity(attr_count);
		CUSTOM_ASSERT(attr_count <= buffer->attributes.capacity, "too many attributes");
		for (u16 attr_i = 0; attr_i < attr_count; ++attr_i) {
			buffer->attributes.push();
			opengl::Attribute * attribute = new (&buffer->attributes[attr_i]) opengl::Attribute;
			attribute->count = *bc.read<u8>();
		}
		if (buffer->is_index) { resource->index_buffer = i; }
	}
}

static void platform_Allocate_Mesh(Bytecode const & bc) {
	u32 asset_id = *bc.read<u32>();
	ogl.meshes_ensure_capacity(asset_id);
	opengl::Mesh * resource = &ogl.meshes.get(asset_id);
	if (resource->id != empty_gl_id) {
		CUSTOM_WARNING("mesh %d already exists", asset_id);
		opengl::Mesh default_resource;
		platform_consume_mesh_params(bc, &default_resource);
		return;
	}
	platform_consume_mesh_params(bc, resource);

	CUSTOM_ASSERT(ogl.version >= COMPILE_VERSION(3, 0), "VAOs are not supported");

	if (ogl.active_mesh != empty_asset_id && ogl.active_mesh != asset_id) {
		ogl.active_mesh = empty_asset_id;
		CUSTOM_WARNING("OGL: disabling active mesh %d for allocation of mesh %d", ogl.active_mesh, asset_id);
	}

	// -- allocate memory --
	if (ogl.version >= COMPILE_VERSION(4, 5)) {
		for (u16 i = 0; i < resource->buffers.count; ++i) {
			opengl::Buffer & buffer = resource->buffers[i];
			GLenum usage = get_mesh_usage(buffer.frequency, buffer.access);
			glCreateBuffers(1, &buffer.id);
			glNamedBufferData(
				buffer.id,
				buffer.capacity * get_type_size(buffer.type),
				NULL, usage
			);
		}
	}
	else {
		for (u16 i = 0; i < resource->buffers.count; ++i) {
			opengl::Buffer & buffer = resource->buffers[i];
			GLenum usage = get_mesh_usage(buffer.frequency, buffer.access);
			GLenum target = buffer.is_index ? GL_ELEMENT_ARRAY_BUFFER : GL_ARRAY_BUFFER;
			glGenBuffers(1, &buffer.id);
			glBindBuffer(target, buffer.id);
			glBufferData(
				target,
				buffer.capacity * get_type_size(buffer.type),
				NULL, usage
			);
		}
	}

	// -- chart memory --
	glGenVertexArrays(1, &resource->id);
	glBindVertexArray(resource->id);
	for (u16 i = 0; i < resource->buffers.count; ++i) {
		opengl::Buffer & buffer = resource->buffers[i];
		GLenum const target = buffer.is_index ? GL_ELEMENT_ARRAY_BUFFER : GL_ARRAY_BUFFER;
		glBindBuffer(target, buffer.id);
	}

	if (ogl.version >= COMPILE_VERSION(4, 3)) {
		for (u16 i = 0; i < resource->buffers.count; ++i) {
			opengl::Buffer & buffer = resource->buffers[i];
			u16 element_size = get_type_size(buffer.type);
			GLenum element_type = get_data_type(buffer.type);

			GLsizei stride = 0;
			for (u8 attr_i = 0; attr_i < buffer.attributes.count; ++attr_i) {
				opengl::Attribute & attr = buffer.attributes[attr_i];
				stride += attr.count * element_size;
			}

			glBindVertexBuffer(i, buffer.id, 0, stride); // glVertexArrayVertexBuffer(resource->id, ...);
			GLuint attrib_offset = 0;
			for (u8 attr_i = 0; attr_i < buffer.attributes.count; ++attr_i) {
				opengl::Attribute & attr = buffer.attributes[attr_i];
				glEnableVertexAttribArray(attr_i); // glEnableVertexArrayAttrib(resource->id, ...);
				glVertexAttribFormat( // glVertexArrayAttribFormat(resource->id, ...);
					attr_i, attr.count, element_type, false, attrib_offset
				);
				glVertexAttribBinding(attr_i, i); // glVertexArrayAttribBinding(resource->id, ...);
				attrib_offset += attr.count * element_size;
			}
		}
	}
	else {
		for (u16 i = 0; i < resource->buffers.count; ++i) {
			opengl::Buffer & buffer = resource->buffers[i];
			u16 element_size = get_type_size(buffer.type);
			GLenum element_type = get_data_type(buffer.type);
	
			GLsizei stride = 0;
			for (u8 attr_i = 0; attr_i < buffer.attributes.count; ++attr_i) {
				opengl::Attribute & attr = buffer.attributes[attr_i];
				stride += attr.count * element_size;
			}
	
			GLenum const target = buffer.is_index ? GL_ELEMENT_ARRAY_BUFFER : GL_ARRAY_BUFFER;
			glBindBuffer(target, buffer.id);
			uptr attrib_offset = 0;
			for (u8 attr_i = 0; attr_i < buffer.attributes.count; ++attr_i) {
				opengl::Attribute & attr = buffer.attributes[attr_i];
				glEnableVertexAttribArray(attr_i);
				glVertexAttribPointer(
					attr_i, attr.count, element_type, false,
					stride, (cmemory)attrib_offset
				);
				attrib_offset += attr.count * element_size;
			}
		}
	}
}

static void platform_consume_target_params(Bytecode const & bc, opengl::Target * resource) {
	new (resource) opengl::Target;
	resource->target = GL_FRAMEBUFFER;

	u16 textures_count = *bc.read<u16>();
	u32 const * texture_ids = bc.read<u32>(textures_count);
	CUSTOM_ASSERT(textures_count <= resource->texture_ids.capacity, "too many textures");
	for (u16 i = 0; i < textures_count; ++i) {
		resource->texture_ids.push(texture_ids[i]);
	}

	u16 buffers_count = *bc.read<u16>();
	CUSTOM_ASSERT(buffers_count <= resource->buffers.capacity, "too many buffers");
	for (u16 i = 0; i < buffers_count; ++i) {
		resource->buffers.push();
		opengl::Render_Buffer * buffer = new (&resource->buffers[i]) opengl::Render_Buffer;
		buffer->target       = GL_RENDERBUFFER;
		buffer->size         = *bc.read<ivec2>();
		buffer->data_type    = *bc.read<Data_Type>();
		buffer->texture_type = *bc.read<Texture_Type>();
	}
}

static void platform_Allocate_Target(Bytecode const & bc) {
	u32   asset_id = *bc.read<u32>();
	ogl.targets_ensure_capacity(asset_id);
	opengl::Target * resource = &ogl.targets.get(asset_id);
	if (resource->id != empty_gl_id) {
		CUSTOM_WARNING("target %d already exists", asset_id);
		opengl::Target default_resource;
		platform_consume_target_params(bc, &default_resource);
		return;
	}
	platform_consume_target_params(bc, resource);

	CUSTOM_ASSERT(ogl.version >= COMPILE_VERSION(3, 0), "frame buffers are not supported");

	glGenFramebuffers(1, &resource->id);

	// @Todo: process mipmap correctly,
	//        process version correctly
	if (ogl.version >= COMPILE_VERSION(4, 5)) {
		for (u16 i = 0; i < resource->texture_ids.count; ++i) {
			u32 texture_id = resource->texture_ids[i];
			opengl::Texture * texture = &ogl.textures.get(texture_id);
			CUSTOM_ASSERT(texture->id != empty_gl_id, "texture doesn't exist");
			GLenum attachment = get_attachment_format(texture->texture_type, (u8)i);
			GLint mipmap = 0; 
			glNamedFramebufferTexture(resource->id, attachment, texture->id, mipmap);
		}
	}
	else {
		if (ogl.active_target != empty_asset_id && ogl.active_target != asset_id) {
			ogl.active_target = empty_asset_id;
			glBindVertexArray(0);
			CUSTOM_WARNING("OGL: disabling active target %d for allocation of target %d", ogl.active_target, asset_id);
		}
		glBindFramebuffer(resource->target, resource->id);
		if (ogl.version >= COMPILE_VERSION(3, 2)) {
			for (u16 i = 0; i < resource->texture_ids.count; ++i) {
				u32 texture_id = resource->texture_ids[i];
				opengl::Texture * texture = &ogl.textures.get(texture_id);
				CUSTOM_ASSERT(texture->id != empty_gl_id, "texture doesn't exist");
				GLenum attachment = get_attachment_format(texture->texture_type, (u8)i);
				GLint mipmap = 0;
				glFramebufferTexture(resource->target, attachment, texture->id, mipmap);
			}
		}
		else {
			for (u16 i = 0; i < resource->texture_ids.count; ++i) {
				u32 texture_id = resource->texture_ids[i];
				opengl::Texture * texture = &ogl.textures.get(texture_id);
				CUSTOM_ASSERT(texture->id != empty_gl_id, "texture doesn't exist");
				GLenum attachment = get_attachment_format(texture->texture_type, (u8)i);
				GLint mipmap = 0;
				glFramebufferTexture2D(resource->target, attachment, texture->target, texture->id, mipmap);
			}
		}
	}

	//
	for (u16 i = 0; i < resource->buffers.count; ++i) {
		opengl::Render_Buffer * buffer = &resource->buffers[i];
		glGenRenderbuffers(1, &buffer->id);
	}

	if (ogl.version >= COMPILE_VERSION(4, 5)) {
		for (u16 i = 0; i < resource->buffers.count; ++i) {
			opengl::Render_Buffer * buffer = &resource->buffers[i];
			GLenum internal_format = get_texture_internal_format(buffer->texture_type, buffer->data_type, 0);
			GLenum attachment = get_attachment_format(buffer->texture_type, (u8)i);
			glNamedRenderbufferStorage(buffer->id, internal_format, buffer->size.x, buffer->size.y);
			glNamedFramebufferRenderbuffer(buffer->id, attachment, buffer->target, buffer->id);
		}
	}
	else {
		// glBindFramebuffer(resource->target, resource->id);
		for (u16 i = 0; i < resource->buffers.count; ++i) {
			opengl::Render_Buffer * buffer = &resource->buffers[i];
			GLenum internal_format = get_texture_internal_format(buffer->texture_type, buffer->data_type, 0);
			GLenum attachment = get_attachment_format(buffer->texture_type, (u8)i);
			glBindRenderbuffer(buffer->target, buffer->id);
			glRenderbufferStorage(buffer->target, internal_format, buffer->size.x, buffer->size.y);
			glFramebufferRenderbuffer(resource->target, attachment, buffer->target, buffer->id);
		}
	}
}

static void platform_Allocate_Unit(Bytecode const & bc) {
	custom::graphics::unit_id asset_id = *bc.read<custom::graphics::unit_id>();
	CUSTOM_ASSERT(asset_id.texture != empty_asset_id, "texture should be specified in order to use a unit");

	u32 existing_unit = find_unit(asset_id.texture, asset_id.sampler, empty_unit);
	if (existing_unit != empty_unit) { return; }

	CUSTOM_ASSERT(asset_id.sampler == empty_asset_id || ogl.version >= COMPILE_VERSION(3, 2), "samplers are not supported");

	if (!graphics::is_allocated_texture(asset_id.texture)) {
		CUSTOM_WARNING("skipping unit (%d : %d): texture is not allocated", asset_id.texture, asset_id.sampler);
		return;
	}

	if (asset_id.sampler != empty_asset_id && !graphics::is_allocated_sampler(asset_id.sampler)) {
		CUSTOM_WARNING("skipping unit (%d : %d): sampler is not allocated", asset_id.texture, asset_id.sampler);
		return;
	}

	// @Todo: rebind to an occupied one?
	u32 unit = find_empty_unit(empty_unit);
	CUSTOM_ASSERT(unit != empty_unit, "no available texture units");
	custom::graphics::unit_id & unit_id = ogl.unit_ids.get(unit);

	unit_id.texture = asset_id.texture;
	opengl::Texture * texture = &ogl.textures.get(asset_id.texture);
	if (ogl.version >= COMPILE_VERSION(4, 5)) {
		glBindTextureUnit(unit, texture->id);
	}
	else {
		glActiveTexture(GL_TEXTURE0 + unit);
		glBindTexture(texture->target, texture->id);
	}

	if (asset_id.sampler != empty_asset_id) {
		opengl::Sampler * sampler = &ogl.samplers.get(asset_id.sampler);
		unit_id.sampler = asset_id.sampler;
		glBindSampler(unit, sampler->id);
	}
}

static void platform_Free_Shader(Bytecode const & bc) {
	u32 asset_id = *bc.read<u32>();
	opengl::Program * resource = &ogl.programs.get(asset_id);
	CUSTOM_ASSERT(resource->id != empty_gl_id, "shader doesn't exist");
	glDeleteProgram(resource->id);
	resource->opengl::Program::~Program();
	if (ogl.active_program == asset_id) {
		ogl.active_program = empty_asset_id;
	}
}

static void platform_Free_Texture(Bytecode const & bc) {
	u32 asset_id = *bc.read<u32>();
	opengl::Texture * resource = &ogl.textures.get(asset_id);
	CUSTOM_ASSERT(resource->id != empty_gl_id, "texture doesn't exist");
	glDeleteTextures(1, &resource->id);

	for (u32 i = 0; i < ogl.unit_ids.count; ++i) {
		custom::graphics::unit_id & it = ogl.unit_ids.get(i);
		if (it.texture == asset_id) {
			it.texture = empty_asset_id;
			// @Note: texture is unbound by deletion
		}
	}

	if (ogl.version >= COMPILE_VERSION(3, 2)) {
		for (u32 i = 0; i < ogl.unit_ids.count; ++i) {
			custom::graphics::unit_id & it = ogl.unit_ids.get(i);
			if (it.sampler != empty_asset_id) {
				it.sampler = empty_asset_id;
				glBindSampler(i, 0);
			}
		}
	}

	resource->opengl::Texture::~Texture();
}

static void platform_Free_Sampler(Bytecode const & bc) {
	u32 asset_id = *bc.read<u32>();
	opengl::Sampler * resource = &ogl.samplers.get(asset_id);
	CUSTOM_ASSERT(resource->id != empty_gl_id, "sampler doesn't exist");
	glDeleteSamplers(1, &resource->id);

	for (u32 i = 0; i < ogl.unit_ids.count; ++i) {
		custom::graphics::unit_id & it = ogl.unit_ids.get(i);
		if (it.sampler == asset_id) {
			it.sampler = empty_asset_id;
			// @Note: sampler is unbound by deletion
		}
	}

	resource->opengl::Sampler::~Sampler();
}

static void platform_Free_Mesh(Bytecode const & bc) {
	u32 asset_id = *bc.read<u32>();
	opengl::Mesh * resource = &ogl.meshes.get(asset_id);
	CUSTOM_ASSERT(resource->id != empty_gl_id, "mesh doesn't exist");
	for (u16 i = 0; i < resource->buffers.count; ++i) {
		glDeleteBuffers(1, &resource->buffers[i].id);
	}
	glDeleteVertexArrays(1, &resource->id);
	resource->opengl::Mesh::~Mesh();
	if (ogl.active_mesh == asset_id) {
		ogl.active_mesh = empty_asset_id;
	}
}

static void platform_Free_Target(Bytecode const & bc) {
	u32 asset_id = *bc.read<u32>();
	opengl::Target * resource = &ogl.targets.get(asset_id);
	CUSTOM_ASSERT(resource->id != empty_gl_id, "target doesn't exist");
	// @Todo: textures fate?
	// for (u16 i = 0; i < resource->textures.count; ++i) {
	// 	glDeleteTextures(1, &resource->textures[i].id);
	// }
	for (u16 i = 0; i < resource->buffers.count; ++i) {
		glDeleteRenderbuffers(1, &resource->buffers[i].id);
	}
	glDeleteFramebuffers(1, &resource->id);
	resource->opengl::Target::~Target();
	if (ogl.active_target == asset_id) {
		ogl.active_target = empty_asset_id;
	}
}

static void platform_Free_Unit(Bytecode const & bc) {
	custom::graphics::unit_id asset_id = *bc.read<custom::graphics::unit_id>();
	CUSTOM_ASSERT(asset_id.texture != empty_asset_id, "texture should be specified in order to suspend a unit");

	u32 unit = find_unit(asset_id.texture, asset_id.sampler, empty_unit);
	CUSTOM_ASSERT(unit != empty_unit, "no such texture unit available");
	custom::graphics::unit_id & unit_id = ogl.unit_ids.get(unit);

	if (asset_id.texture != empty_asset_id) {
		unit_id.texture = empty_asset_id;
		if (ogl.version >= COMPILE_VERSION(4, 5)) {
			glBindTextureUnit(unit, 0);
		}
		else {
			opengl::Texture * texture = &ogl.textures.get(asset_id.texture);
			glActiveTexture(GL_TEXTURE0 + unit);
			glBindTexture(texture->target, 0);
		}
	}

	if (asset_id.sampler != empty_asset_id) {
		CUSTOM_ASSERT(ogl.version >= COMPILE_VERSION(3, 2), "samplers are not supported");
		unit_id.sampler = empty_asset_id;
		glBindSampler(unit, 0);
	}
}

static void platform_Use_Shader(Bytecode const & bc) {
	u32 asset_id = *bc.read<u32>();
	if (ogl.active_program != asset_id) {
		ogl.active_program = asset_id;
	}

	if (asset_id == empty_asset_id) {
		glUseProgram(0);
		return;
	}

	if (!graphics::is_allocated_shader(asset_id)) {
		CUSTOM_WARNING("skipping shader %d: it is not allocated", asset_id);
		return;
	}
	opengl::Program const * resource = &ogl.programs.get(asset_id);
	glUseProgram(resource->id);
}

static void platform_Use_Mesh(Bytecode const & bc) {
	u32 asset_id = *bc.read<u32>();
	if (ogl.active_mesh != asset_id) {
		ogl.active_mesh = asset_id;
	}

	if (asset_id == empty_asset_id) {
		glBindVertexArray(0);
		return;
	}

	if (!graphics::is_allocated_mesh(asset_id)) {
		CUSTOM_WARNING("skipping mesh %d: it is not allocated", asset_id);
		return;
	}
	opengl::Mesh const * resource = &ogl.meshes.get(asset_id);
	glBindVertexArray(resource->id);
	// opengl::Buffer const & indices = resource->buffers[resource->index_buffer];
	// glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices.id);
}

static void platform_Use_Target(Bytecode const & bc) {
	u32 asset_id = *bc.read<u32>();
	if (ogl.active_target != asset_id) {
		ogl.active_target = asset_id;
	}

	if (asset_id == empty_asset_id) {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		return;
	}

	opengl::Target const * resource = &ogl.targets.get(asset_id);
	CUSTOM_ASSERT(resource->id != empty_gl_id, "target doesn't exist");
	glBindFramebuffer(resource->target, resource->id);
}

static void platform_Load_Shader(Bytecode const & bc) {
	// @Change: receive a pointer instead, then free if needed?
	u32 asset_id = *bc.read<u32>();
	opengl::Program * resource = &ogl.programs.get(asset_id);
	CUSTOM_ASSERT(resource->id != empty_gl_id, "shader doesn't exist");

	Inline_String source = read_cstring(bc);
	Shader_Part parts = *bc.read<Shader_Part>();
	if (resource->uploaded) { return; }
	resource->uploaded = true;

	platform_link_program(resource->id, {(GLint)source.count, source.data}, parts);

	// CUSTOM_TRACE("program %d info:", asset_id);
	Program_Field field_buffer;

	// GLint attributes_capacity;
	// glGetProgramiv(resource->id, GL_ACTIVE_ATTRIBUTES, &attributes_capacity);
	// // resource->attributes.set_capacity(attributes_capacity);
	// for (GLint i = 0; i < attributes_capacity; ++i) {
	// 	// resource->attributes.push();
	// 	// opengl::Field * field = new (&resource->attributes[i]) opengl::Field;
	// 	platform_get_active_attribute(resource->id, i, field_buffer);
	// 	// CUSTOM_TRACE(
	// 	// 	"  - attribute 0x%x '%s' [%d]; // ind %d, loc %d",
	// 	// 	field_buffer.type, field_buffer.name, field_buffer.size,
	// 	// 	i, field_buffer.location
	// 	// );
	// 	// field->id = find_attribute_id(field_buffer.name);
	// 	// field->location = field_buffer.location;
	// }

	GLint uniforms_count;
	glGetProgramiv(resource->id, GL_ACTIVE_UNIFORMS, &uniforms_count);
	CUSTOM_ASSERT(uniforms_count <= resource->uniforms.capacity, "too many uniforms");
	// resource->uniforms.set_capacity(uniforms_count);
	for (GLint i = 0; i < uniforms_count; ++i) {
		resource->uniforms.push();
		opengl::Field * field = new (&resource->uniforms[(u16)i]) opengl::Field;
		platform_get_active_uniform(resource->id, i, field_buffer);
		// CUSTOM_TRACE(
		// 	"  - uniform 0x%x '%s' [%d]; // ind %d, loc %d",
		// 	field_buffer.type, field_buffer.name, field_buffer.size,
		// 	i, field_buffer.location
		// );
		field->id = find_uniform_id(field_buffer.name, field_buffer.name_count);
		field->location = field_buffer.location;
	}
}

static void platform_Load_Texture(Bytecode const & bc) {
	// @Change: receive a pointer instead, then free if needed?
	u32 asset_id = *bc.read<u32>();
	opengl::Texture * resource = &ogl.textures.get(asset_id);
	CUSTOM_ASSERT(resource->id != empty_gl_id, "texture doesn't exist");

	ivec2 offset = *bc.read<ivec2>();
	ivec2 size = *bc.read<ivec2>();
	u8           channels     = *bc.read<u8>();
	Data_Type    data_type    = *bc.read<Data_Type>();
	Texture_Type texture_type = *bc.read<Texture_Type>();
	cmemory data = read_data(bc, data_type, size.x * size.y * channels);
	if (resource->uploaded) {
		if (!resource->is_dynamic) { return; }
		CUSTOM_TRACE("overwriting texture %d data", asset_id);
	}
	resource->uploaded = true;

	CUSTOM_ASSERT(offset.x + size.x <= resource->size.x, "texture %d error: writing past data x bounds", asset_id);
	CUSTOM_ASSERT(offset.y + size.y <= resource->size.y, "texture %d error: writing past data y bounds", asset_id);
	CUSTOM_ASSERT(channels == resource->channels, "texture %d error: different channels count", asset_id)
	CUSTOM_ASSERT(data_type == resource->data_type, "texture %d error: different data types", asset_id)
	CUSTOM_ASSERT(texture_type == resource->texture_type, "texture %d error: different texture types", asset_id)

	if (ogl.version >= COMPILE_VERSION(4, 5)) {
		glTextureSubImage2D(
			resource->id,
			0,
			offset.x, offset.y, size.x, size.y,
			get_texture_data_format(texture_type, channels),
			get_texture_data_type(texture_type, data_type),
			data
		);
	}
	else {
		glBindTexture(resource->target, resource->id);
		glTexSubImage2D(
			resource->target,
			0,
			offset.x, offset.y, size.x, size.y,
			get_texture_data_format(texture_type, channels),
			get_texture_data_type(texture_type, data_type),
			data
		);
	}
}

static void platform_Load_Mesh(Bytecode const & bc) {
	// @Change: receive a pointer instead, then free if needed?
	u32 asset_id = *bc.read<u32>();
	opengl::Mesh * resource = &ogl.meshes.get(asset_id);
	CUSTOM_ASSERT(resource->id != empty_gl_id, "mesh doesn't exist");

	u8 buffers_count = *bc.read<u8>();

	if (ogl.version >= COMPILE_VERSION(4, 5)) {
		for (u16 i = 0; i < buffers_count; ++i) {
			opengl::Buffer & buffer = resource->buffers[i];
			GLenum usage = get_mesh_usage(buffer.frequency, buffer.access);
			GLenum const target = buffer.is_index ? GL_ELEMENT_ARRAY_BUFFER : GL_ARRAY_BUFFER;

			u32 offset = *bc.read<u32>();
			DT_Array in_buffer = read_data_array(bc);
			if (resource->uploaded) {
				if (buffer.frequency == custom::graphics::Mesh_Frequency::Static) { continue; }
				CUSTOM_TRACE("overwriting mesh %d data", asset_id);
			}

			CUSTOM_ASSERT(offset + in_buffer.count <= buffer.capacity, "mesh %d buffer %d error: writing past data bounds", asset_id, i);
			CUSTOM_ASSERT(buffer.type == in_buffer.type, "mesh %d buffer %d error: different data types", asset_id, i);

			GLsizeiptr type_size = get_type_size(in_buffer.type);

			glNamedBufferSubData(
				buffer.id,
				offset * type_size,
				in_buffer.count * type_size,
				in_buffer.data
			);
		}
	}
	else {
		if (ogl.active_mesh != empty_asset_id && ogl.active_mesh != asset_id) {
			ogl.active_mesh = empty_asset_id;
			glBindVertexArray(0);
			CUSTOM_WARNING("OGL: disabling active mesh %d for loading of mesh %d", ogl.active_mesh, asset_id);
		}
		for (u16 i = 0; i < buffers_count; ++i) {
			opengl::Buffer & buffer = resource->buffers[i];
			GLenum usage = get_mesh_usage(buffer.frequency, buffer.access);
			GLenum const target = buffer.is_index ? GL_ELEMENT_ARRAY_BUFFER : GL_ARRAY_BUFFER;
	
			u32 offset = *bc.read<u32>();
			DT_Array in_buffer = read_data_array(bc);
			if (resource->uploaded) {
				if (buffer.frequency == custom::graphics::Mesh_Frequency::Static) { continue; }
				CUSTOM_TRACE("overwriting mesh %d data", asset_id);
			}
	
			CUSTOM_ASSERT(offset + in_buffer.count <= buffer.capacity, "mesh %d buffer %d error: writing past data bounds", asset_id, i);
			CUSTOM_ASSERT(buffer.type == in_buffer.type, "mesh %d buffer %d error: different data types", asset_id, i);
	
			GLsizeiptr type_size = get_type_size(in_buffer.type);
	
			glBindBuffer(target, buffer.id);
			glBufferSubData(
				target,
				offset * type_size,
				in_buffer.count * type_size,
				in_buffer.data
			);
		}
	}
	resource->uploaded = true;
}

static void platform_Set_Uniform(Bytecode const & bc) {
	u32 asset_id = *bc.read<u32>();
	if (!graphics::is_allocated_shader(asset_id)) {
		CUSTOM_WARNING("skipping shader %d: it is not allocated", asset_id);
		bc.read<u32>();
		read_data_array(bc);
		return;
	}
	opengl::Program const * resource = &ogl.programs.get(asset_id);

	u32 uniform_id = *bc.read<u32>();
	DT_Array uniform = read_data_array(bc);

	opengl::Field const * field = find_uniform_field(asset_id, uniform_id);

	// @Todo: compare types and size?

	// @Note: even though glBindTextureUnit(...) and glBindSampler(...) takes GLuint as a unit,
	//        glUniform1i and glUniform1iv are the only two functions that may be used to load uniform variables defined as sampler types. Loading samplers with any other function will result in a GL_INVALID_OPERATION error.
	//        https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glUniform.xhtml

	// @Todo: cache units, then assign uniforms at once?

	static custom::Array_Fixed<s32, 256> units; units.count = 0;
	if (ogl.version >= COMPILE_VERSION(4, 1)) {
		switch (uniform.type) {
			case Data_Type::unit_id: {
				custom::graphics::unit_id const * unit_ids = (custom::graphics::unit_id *)uniform.data;
				for (u32 i = 0; i < uniform.count; ++i) {
					custom::graphics::unit_id unit_id = unit_ids[i];
					u32 unit = find_unit(unit_id.texture, unit_id.sampler, ogl.unit_ids.capacity - 1);
					units.push((s32)unit);
				}
				glProgramUniform1iv(resource->id, field->location, units.count, units.data);
			} break;
			//
			case Data_Type::r32:  glProgramUniform1fv(resource->id, field->location, uniform.count, (r32 *)uniform.data); break;
			case Data_Type::vec2: glProgramUniform2fv(resource->id, field->location, uniform.count, (r32 *)uniform.data); break;
			case Data_Type::vec3: glProgramUniform3fv(resource->id, field->location, uniform.count, (r32 *)uniform.data); break;
			case Data_Type::vec4: glProgramUniform4fv(resource->id, field->location, uniform.count, (r32 *)uniform.data); break;
			//
			case Data_Type::s32:   glProgramUniform1iv(resource->id, field->location, uniform.count, (s32 *)uniform.data); break;
			case Data_Type::ivec2: glProgramUniform2iv(resource->id, field->location, uniform.count, (s32 *)uniform.data); break;
			case Data_Type::ivec3: glProgramUniform3iv(resource->id, field->location, uniform.count, (s32 *)uniform.data); break;
			case Data_Type::ivec4: glProgramUniform4iv(resource->id, field->location, uniform.count, (s32 *)uniform.data); break;
			//
			case Data_Type::u32:   glProgramUniform1uiv(resource->id, field->location, uniform.count, (u32 *)uniform.data); break;
			case Data_Type::uvec2: glProgramUniform2uiv(resource->id, field->location, uniform.count, (u32 *)uniform.data); break;
			case Data_Type::uvec3: glProgramUniform3uiv(resource->id, field->location, uniform.count, (u32 *)uniform.data); break;
			case Data_Type::uvec4: glProgramUniform4uiv(resource->id, field->location, uniform.count, (u32 *)uniform.data); break;
			//
			case Data_Type::mat2: glProgramUniformMatrix2fv(resource->id, field->location, uniform.count, GL_FALSE, (r32 *)uniform.data); break;
			case Data_Type::mat3: glProgramUniformMatrix3fv(resource->id, field->location, uniform.count, GL_FALSE, (r32 *)uniform.data); break;
			case Data_Type::mat4: glProgramUniformMatrix4fv(resource->id, field->location, uniform.count, GL_FALSE, (r32 *)uniform.data); break;
		}
	}
	else {
		if (ogl.active_program != empty_asset_id && ogl.active_program != asset_id) {
			glUseProgram(resource->id);
			CUSTOM_WARNING("OGL: switching from active program %d for setting uniforms of program %d", ogl.active_program, asset_id);
		}
		switch (uniform.type) {
			case Data_Type::unit_id: {
				custom::graphics::unit_id const * unit_ids = (custom::graphics::unit_id *)uniform.data;
				for (u32 i = 0; i < uniform.count; ++i) {
					custom::graphics::unit_id unit_id = unit_ids[i];
					u32 unit = find_unit(unit_id.texture, unit_id.sampler, ogl.unit_ids.capacity - 1);
					units.push((s32)unit);
				}
				glUniform1iv(field->location, units.count, units.data);
			} break;
			//
			case Data_Type::r32:  glUniform1fv(field->location, uniform.count, (r32 *)uniform.data); break;
			case Data_Type::vec2: glUniform2fv(field->location, uniform.count, (r32 *)uniform.data); break;
			case Data_Type::vec3: glUniform3fv(field->location, uniform.count, (r32 *)uniform.data); break;
			case Data_Type::vec4: glUniform4fv(field->location, uniform.count, (r32 *)uniform.data); break;
			//
			case Data_Type::s32:   glUniform1iv(field->location, uniform.count, (s32 *)uniform.data); break;
			case Data_Type::ivec2: glUniform2iv(field->location, uniform.count, (s32 *)uniform.data); break;
			case Data_Type::ivec3: glUniform3iv(field->location, uniform.count, (s32 *)uniform.data); break;
			case Data_Type::ivec4: glUniform4iv(field->location, uniform.count, (s32 *)uniform.data); break;
			//
			case Data_Type::u32:   glUniform1uiv(field->location, uniform.count, (u32 *)uniform.data); break;
			case Data_Type::uvec2: glUniform2uiv(field->location, uniform.count, (u32 *)uniform.data); break;
			case Data_Type::uvec3: glUniform3uiv(field->location, uniform.count, (u32 *)uniform.data); break;
			case Data_Type::uvec4: glUniform4uiv(field->location, uniform.count, (u32 *)uniform.data); break;
			//
			case Data_Type::mat2: glUniformMatrix2fv(field->location, uniform.count, GL_FALSE, (r32 *)uniform.data); break;
			case Data_Type::mat3: glUniformMatrix3fv(field->location, uniform.count, GL_FALSE, (r32 *)uniform.data); break;
			case Data_Type::mat4: glUniformMatrix4fv(field->location, uniform.count, GL_FALSE, (r32 *)uniform.data); break;
		}
		if (ogl.active_program != empty_asset_id && ogl.active_program != asset_id) {
			opengl::Program const * active_program = &ogl.programs.get(ogl.active_program);
			glUseProgram(active_program->id);
			CUSTOM_WARNING("OGL: switching to active program %d after setting uniforms of program %d", ogl.active_program, asset_id);
		}
	}
}

static void platform_Viewport(Bytecode const & bc) {
	ivec2 pos  = *bc.read<ivec2>();
	ivec2 size = *bc.read<ivec2>();
	glViewport(pos.x, pos.y, size.x, size.y);
}

static void platform_Clear(Bytecode const & bc) {
	GLbitfield gl_clear_flags = 0;
	Clear_Flags clear_flags = *bc.read<Clear_Flags>();
	if (bits_are_set(clear_flags, Clear_Flags::Color)) {
		gl_clear_flags |= GL_COLOR_BUFFER_BIT;
	}
	if (bits_are_set(clear_flags, Clear_Flags::Depth)) {
		gl_clear_flags |= GL_DEPTH_BUFFER_BIT;
	}
	if (bits_are_set(clear_flags, Clear_Flags::Stencil)) {
		gl_clear_flags |= GL_STENCIL_BUFFER_BIT;
	}
	glClear(gl_clear_flags);
}

static void platform_Draw(Bytecode const & bc) {
	if (ogl.active_program == empty_asset_id) {
		CUSTOM_WARNING("skipping draw: no active program");
		return;
	}
	
	if (ogl.active_mesh == empty_asset_id) {
		CUSTOM_WARNING("skipping draw: no active mesh");
		return;
	}

	opengl::Program const * program = &ogl.programs.get(ogl.active_program);
	opengl::Mesh const * mesh = &ogl.meshes.get(ogl.active_mesh);

	// // GLint program_id;
	// // glGetIntegerv(GL_CURRENT_PROGRAM, &program_id);
	// glValidateProgram(program->id);
	// platform_verify_program(program->id, GL_VALIDATE_STATUS);

	opengl::Buffer const & indices = mesh->buffers[mesh->index_buffer];
	GLenum data_type = get_data_type(indices.type);
	glDrawElements(GL_TRIANGLES, indices.count, data_type, nullptr);
}

static void platform_Overlay(Bytecode const & bc) {
	// send to a vertex shader indices [0, 1, 2]
	glDrawArrays(GL_TRIANGLES, 0, 3);
	// https://rauwendaal.net/2014/06/14/rendering-a-screen-covering-triangle-in-opengl/
	// https://twitter.com/nice_byte/status/1093355080235999232
}

//
//
//

static void platform_Init_Uniforms(Bytecode const & bc) {
	u32 const name_capacity = C_ARRAY_LENGTH(Program_Field::name);
	u32 count = *bc.read<u32>();
	ogl.uniform_names.set_capacity(count * name_capacity); ogl.uniform_names.count = 0;
	ogl.uniform_names_lengths.set_capacity(count); ogl.uniform_names_lengths.count = 0;
	for (u32 i = 0; i < count; ++i) {
		Inline_String name = read_cstring(bc);
		ogl.uniform_names.push_range(name.data, name.count);
		ogl.uniform_names_lengths.push(name.count);
	}
}

static void platform_Message_Pointer(Bytecode const & bc) {
	cstring message = *bc.read<cstring>();
	CUSTOM_TRACE("OpenGL VM: %s", message);
}

static void platform_Message_Inline(Bytecode const & bc) {
	Inline_String message = read_cstring(bc);
	CUSTOM_TRACE("OpenGL VM: %s", message.data);
}

}}
