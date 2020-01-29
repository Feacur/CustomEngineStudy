#include "custom_pch.h"
#include "engine/core/math_types.h"
#include "engine/api/graphics_vm.h"
#include "engine/api/graphics_params.h"
#include "engine/impl/array.h"
#include "engine/impl/array_fixed.h"
#include "engine/impl/bytecode.h"

#if !defined(CUSTOM_PRECOMPILED_HEADER)
	#include <glad/glad.h>
	#include <new>
#endif

// https://www.khronos.org/registry/OpenGL/index_gl.php

// https://github.com/etodd/lasercrabs/blob/master/src/platform/glvm.cpp

typedef GLchar const * glstring;

struct Shader_Props
{
	GLenum  type;
	cstring version;
	cstring defines;
};

namespace opengl {

struct Program
{
	GLuint id;
	custom::Array<GLuint> uniforms; // @Todo: use fixed?
};
template struct custom::Array<Program>;

struct Texture
{
	GLuint id;
};
template struct custom::Array<Texture>;

struct Attribute
{
	GLenum type;
	GLint  count;
};
template struct custom::Array<Attribute>; // @Todo: use fixed?

struct Buffer
{
	GLuint id;
	custom::Array<Attribute> attributes; // @Todo: use fixed?
};
template struct custom::Array<Buffer>; // @Todo: use fixed?

struct Indices
{
	GLuint id;
	u32 count;
};

struct Mesh
{
	GLuint id;
	custom::Array<Buffer> buffers; // @Todo: use fixed?
	Indices indices;
};
template struct custom::Array<Mesh>;

struct Data
{
	custom::Array<Program> programs;
	custom::Array<Texture> textures;
	custom::Array<Mesh>    meshes;
};

}

static opengl::Data ogl;

//
// API implementation
//

static void opengl_message_callback(
	GLenum source, GLenum type, GLuint id, GLenum severity,
	GLsizei length, glstring message, void const * userParam
);
static GLuint create_program(cstring source);

namespace custom {
namespace graphics {

static void consume_single_instruction(Bytecode const & bc);

VM::VM()
{
	#if !defined(GES_SHIPPING)
	GLint version_major;
	glGetIntegerv(GL_MAJOR_VERSION, &version_major);
	GLint version_minor;
	glGetIntegerv(GL_MINOR_VERSION, &version_minor);
	CUSTOM_MESSAGE("OpenGL version %d.%d", version_major, version_minor);

	if (version_major == 4 && version_minor >= 3 || version_major > 4) {
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(opengl_message_callback, nullptr);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_FALSE);
	}
	#endif

	// glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	// glDepthRangef(0.0f, 1.0f);
	// glClearDepth(1.0f);
	// glFrontFace(GL_CCW);
}

VM::~VM() = default;

void VM::update(Bytecode const & bc)
{
	while (bc.offset < bc.buffer.count) {
		consume_single_instruction(bc);
		#if !defined(CUSTOM_SHIPPING)
			static GLenum error = 0;
			while ((error = glGetError()) != GL_NO_ERROR) {
				CUSTOM_ASSERT(false, "OpenGL error 0x%x", error);
			}
		#endif
	}
}

}}

//
// instruction implementation
//

namespace custom {
namespace graphics {

static GLenum get_comparison(Comparison value) {
	switch (value) {
		case Comparison::False:   return GL_NONE;
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

static GLenum get_internal_format(Texture_Type texture_type, Data_Type data_type, u8 channels) {
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

static GLenum get_data_format(Texture_Type texture_type, u8 channels) {
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

static GLenum get_data_type(Texture_Type texture_type, Data_Type data_type) {
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

// #define CASE_IMPL(T) case Data_Type::T: return sizeof(T)
// static u8 get_data_type_size(Data_Type value) {
// 	switch (value) {
// 		case Data_Type::tex: return sizeof(s32);
// 		CASE_IMPL(s8); CASE_IMPL(s16); CASE_IMPL(s32);
// 		CASE_IMPL(u8); CASE_IMPL(u16); CASE_IMPL(u32);
// 		CASE_IMPL(r32); CASE_IMPL(r64);
// 		CASE_IMPL(vec2); CASE_IMPL(vec3); CASE_IMPL(vec4);
// 		CASE_IMPL(ivec2); CASE_IMPL(ivec3); CASE_IMPL(ivec4);
// 		CASE_IMPL(uvec2); CASE_IMPL(uvec3); CASE_IMPL(uvec4);
// 		CASE_IMPL(mat2); CASE_IMPL(mat3); CASE_IMPL(mat4);
// 	}
// 	CUSTOM_ASSERT(false, "unknown data type %d", value);
// 	return 0;
// }
// #undef CASE_IMPL

#define CASE_IMPL(T) case Data_Type::T: return bc.read<T>(count)
static void const * read_inline(Bytecode const & bc, Data_Type type, u32 count) {
	switch (type) {
		case Data_Type::tex: return bc.read<s32>();
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

static void consume_single_instruction(Bytecode const & bc)
{
	Instruction instruction = *bc.read<Instruction>();
	switch (instruction)
	{
		//
		case Instruction::Viewport: {
			ivec2 pos  = *bc.read<ivec2>();
			ivec2 size = *bc.read<ivec2>();
			glViewport(pos.x, pos.y, size.x, size.y);
		} return;

		case Instruction::Clear: {
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
		} return;

		case Instruction::Depth_Read: {
			u8 value = *bc.read<u8>();
			if (value) {
				glEnable(GL_DEPTH_TEST);
			}
			else {
				glDisable(GL_DEPTH_TEST);
			}
		} return;

		case Instruction::Depth_Write: {
			u8 value = *bc.read<u8>();
			glDepthMask(value);
		} return;

		case Instruction::Depth_Comparison: {
			Comparison value = *bc.read<Comparison>();
			glDepthFunc(get_comparison(value));
		} return;

		case Instruction::Color_Write: {
			Color_Write value = *bc.read<Color_Write>();
			glColorMask(
				bits_are_set(value, Color_Write::R),
				bits_are_set(value, Color_Write::G),
				bits_are_set(value, Color_Write::B),
				bits_are_set(value, Color_Write::A)
			);
		} return;

		case Instruction::Stencil_Read: {
			u8 value = *bc.read<u8>();
			if (value) {
				glEnable(GL_STENCIL_TEST);
			}
			else {
				glDisable(GL_STENCIL_TEST);
			}
		} return;

		case Instruction::Stencil_Write: {
			u32 value = *bc.read<u32>();
			glStencilMask(value);
		} return;

		case Instruction::Stencil_Comparison: {
			Comparison value = *bc.read<Comparison>();
			u8 ref  = *bc.read<u8>();
			u8 mask = *bc.read<u8>();
			glStencilFunc(get_comparison(value), ref, mask);
		} return;

		case Instruction::Stencil_Operation: {
			// stencil test + depth test
			Operation fail_any  = *bc.read<Operation>();
			Operation succ_fail = *bc.read<Operation>();
			Operation succ_succ = *bc.read<Operation>();
			glStencilOp(
				get_operation(fail_any),
				get_operation(succ_fail),
				get_operation(succ_succ)
			);
		} return;

		case Instruction::Stencil_Mask: {
			u8 mask = *bc.read<u8>();
			glStencilMask(mask);
		} return;

		case Instruction::Blend_Mode: {
			Blend_Mode value = *bc.read<Blend_Mode>();

			if (value == Blend_Mode::Opaque) {
				glDisable(GL_BLEND);
			}
			else {
				glEnable(GL_BLEND);
			}

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
		} return;

		case Instruction::Cull_Mode: {
			Cull_Mode value = *bc.read<Cull_Mode>();

			if (value == Cull_Mode::None) {
				glDisable(GL_CULL_FACE);
			}
			else {
				glEnable(GL_CULL_FACE);
			}

			switch (value) {
				case Cull_Mode::Back:
					glCullFace(GL_BACK);
					return;
				case Cull_Mode::Front:
					glCullFace(GL_FRONT);
					return;
				case Cull_Mode::Both:
					glCullFace(GL_FRONT_AND_BACK);
					return;
			}

			CUSTOM_ASSERT(false, "unknown cull mode %d", value);
		} return;

		//
		case Instruction::Prepare_Uniform: {
			CUSTOM_MESSAGE("// @Todo: Prepare_Uniform");
		} return;

		//
		case Instruction::Allocate_Shader: {
			u32     asset_id = *bc.read<u32>();
			u32     length   = *bc.read<u32>();
			cstring source   =  bc.read<char>(length);

			ogl.programs.ensure_capacity(asset_id + 1);
			opengl::Program * resource = new (&ogl.programs[asset_id]) opengl::Program;

			resource->id = create_program(source);

			// @Todo: process uniforms
			// GLint uniform_location = glGetUniformLocation(id, uniform_name);
		} return;

		case Instruction::Allocate_Texture: {
			u32   asset_id = *bc.read<u32>();
			ivec2 size     = *bc.read<ivec2>();
			u8           channels     = *bc.read<u8>();
			Data_Type    data_type    = *bc.read<Data_Type>();
			Texture_Type texture_type = *bc.read<Texture_Type>();
			Filter_Mode  texture_filter = *bc.read<Filter_Mode>();
			Filter_Mode  mipmap_filter  = *bc.read<Filter_Mode>();
			Wrap_Mode    wrap_mode_x = *bc.read<Wrap_Mode>();
			Wrap_Mode    wrap_mode_y = *bc.read<Wrap_Mode>();

			ogl.textures.ensure_capacity(asset_id + 1);
			opengl::Texture * resource = new (&ogl.textures[asset_id]) opengl::Texture;

			glCreateTextures(GL_TEXTURE_2D, 1, &resource->id);
			glTextureStorage2D(
				resource->id, 1,
				get_internal_format(texture_type, data_type, channels),
				size.x, size.y
			);

			glTextureParameteri(resource->id, GL_TEXTURE_MIN_FILTER, get_min_filter(texture_filter, mipmap_filter));
			glTextureParameteri(resource->id, GL_TEXTURE_MAG_FILTER, get_mag_filter(texture_filter));
			glTextureParameteri(resource->id, GL_TEXTURE_WRAP_S, get_wrap_mode(wrap_mode_x));
			glTextureParameteri(resource->id, GL_TEXTURE_WRAP_T, get_wrap_mode(wrap_mode_y));
		} return;

		case Instruction::Allocate_Mesh: {
			u32 asset_id = *bc.read<u32>();
			u32          vcount   = *bc.read<u32>();
			void const * vertices = read_inline(bc, Data_Type::r32, vcount);
			u32          icount  = *bc.read<u32>();
			void const * indices = read_inline(bc, Data_Type::u32, icount);

			ogl.meshes.ensure_capacity(asset_id + 1);
			opengl::Mesh * resource = new (&ogl.meshes[asset_id]) opengl::Mesh;

			resource->buffers.push();
			glCreateBuffers(1, &resource->buffers[0].id);
			glBindBuffer(GL_ARRAY_BUFFER, resource->buffers[0].id);
			glBufferData(GL_ARRAY_BUFFER, vcount * sizeof(r32), vertices, GL_STATIC_DRAW);

			resource->indices.count = icount;
			glCreateBuffers(1, &resource->indices.id);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, resource->indices.id);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, icount * sizeof(u32), indices, GL_STATIC_DRAW);

			// @Todo: correctly chart attributes
			glGenVertexArrays(1, &resource->id);
			glBindVertexArray(resource->id);

			glBindBuffer(GL_ARRAY_BUFFER, resource->buffers[0].id);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, resource->indices.id);

			u32 cc1 = 3;
			u32 cc2 = 2;
			u32 stride = (cc1 + cc2) * sizeof(r32);

			uintptr_t attrib_offset = 0;
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(
				0,
				cc1,
				GL_FLOAT,
				false,
				stride,
				(void const *)attrib_offset
			);

			attrib_offset += cc1 * sizeof(r32);
			
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(
				1,
				cc2,
				GL_FLOAT,
				false,
				stride,
				(void const *)attrib_offset
			);
		} return;

		//
		case Instruction::Free_Shader: {
			u32 asset_id = *bc.read<u32>();

			opengl::Program * resource = &ogl.programs[asset_id];
			glDeleteProgram(resource->id);
			resource->opengl::Program::~Program();
		} return;

		case Instruction::Free_Texture: {
			u32 asset_id = *bc.read<u32>();

			opengl::Texture * resource = &ogl.textures[asset_id];
			glDeleteTextures(1, &resource->id);
			resource->opengl::Texture::~Texture();
		} return;

		case Instruction::Free_Mesh: {
			u32 asset_id = *bc.read<u32>();

			opengl::Mesh * resource = &ogl.meshes[asset_id];
			for (u32 i = 0; i < resource->buffers.count; ++i) {
				glDeleteBuffers(GL_ARRAY_BUFFER, &resource->buffers[i].id);
			}
			glDeleteBuffers(GL_ELEMENT_ARRAY_BUFFER, &resource->indices.id);
			glDeleteVertexArrays(1, &resource->id);
			resource->opengl::Mesh::~Mesh();
		} return;

		//
		case Instruction::Use_Shader: {
			u32 asset_id = *bc.read<u32>();

			opengl::Program * resource = &ogl.programs[asset_id];
			glUseProgram(resource->id);
		} return;

		case Instruction::Use_Texture: {
			u32 asset_id = *bc.read<u32>();
			s32 slot     = *bc.read<s32>();

			opengl::Texture * resource = &ogl.textures[asset_id];
			glBindTextureUnit(slot, resource->id);
		} return;

		case Instruction::Use_Mesh: {
			u32 asset_id = *bc.read<u32>();

			opengl::Mesh * resource = &ogl.meshes[asset_id];
			glBindVertexArray(resource->id);
			// glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, resource->indices.id);
		} return;

		//
		case Instruction::Load_Uniform: {
			// @Todo: automize location with some asset_id
			s32 location = *bc.read<s32>();
			Data_Type type = *bc.read<Data_Type>();
			u32 count = *bc.read<u32>();
			void const * data = read_inline(bc, type, count);

			switch (type) {
				case Data_Type::tex: glUniform1iv(location, count, (s32 *)data); break;

				case Data_Type::r32:  glUniform1fv(location, count, (r32 *)data); break;
				case Data_Type::vec2: glUniform2fv(location, count, (r32 *)data); break;
				case Data_Type::vec3: glUniform3fv(location, count, (r32 *)data); break;
				case Data_Type::vec4: glUniform4fv(location, count, (r32 *)data); break;

				case Data_Type::s32:   glUniform1iv(location, count, (s32 *)data); break;
				case Data_Type::ivec2: glUniform2iv(location, count, (s32 *)data); break;
				case Data_Type::ivec3: glUniform3iv(location, count, (s32 *)data); break;
				case Data_Type::ivec4: glUniform4iv(location, count, (s32 *)data); break;

				case Data_Type::u32:   glUniform1uiv(location, count, (u32 *)data); break;
				case Data_Type::uvec2: glUniform2uiv(location, count, (u32 *)data); break;
				case Data_Type::uvec3: glUniform3uiv(location, count, (u32 *)data); break;
				case Data_Type::uvec4: glUniform4uiv(location, count, (u32 *)data); break;

				case Data_Type::mat2: glUniformMatrix2fv(location, count, false, (r32 *)data); break;
				case Data_Type::mat3: glUniformMatrix3fv(location, count, false, (r32 *)data); break;
				case Data_Type::mat4: glUniformMatrix4fv(location, count, false, (r32 *)data); break;
			}
		} return;

		case Instruction::Load_Texture: {
			u32   asset_id = *bc.read<u32>();
			ivec2 size     = *bc.read<ivec2>();
			u8           channels     = *bc.read<u8>();
			Data_Type    data_type    = *bc.read<Data_Type>();
			Texture_Type texture_type = *bc.read<Texture_Type>();
			// @Change: receive a pointer instead, then free if needed?
			void const * data = read_inline(bc, data_type, size.x * size.y * channels);

			opengl::Texture * resource = &ogl.textures[asset_id];
			glTextureSubImage2D(
				resource->id,
				0,
				0, 0, size.x, size.y,
				get_data_format(texture_type, channels),
				get_data_type(texture_type, data_type),
				data
			);
		} return;

		//
		case Instruction::Draw: {
			u32 asset_id = *bc.read<u32>();

			opengl::Mesh * resource = &ogl.meshes[asset_id];
			glDrawElements(GL_TRIANGLES, resource->indices.count, GL_UNSIGNED_INT, nullptr);
		} return;

		case Instruction::Overlay: {
			// send to a vertex shader indices [0, 1, 2]
			glDrawArrays(GL_TRIANGLES, 0, 3);
			// https://rauwendaal.net/2014/06/14/rendering-a-screen-covering-triangle-in-opengl/
			// https://twitter.com/nice_byte/status/1093355080235999232
		} return;
	}

	// message
	switch (instruction)
	{
		case Instruction::Message_Pointer: {
			cstring message = *bc.read<cstring>();
			CUSTOM_MESSAGE("OpenGL VM: %s", message);
		} return;

		case Instruction::Message_Inline: {
			u32 length = *bc.read<u32>();
			cstring message = bc.read<char>(length);
			CUSTOM_MESSAGE("OpenGL VM: %s", message);
		} return;
	}

	// error
	switch (instruction)
	{
		case Instruction::None: {
			CUSTOM_ASSERT(false, "null instruction encountered");
		} return;

		case Instruction::Last: {
			CUSTOM_ASSERT(false, "non-instruction encountered");
		} return;
	}

	if (instruction < Instruction::Last) {
		CUSTOM_ASSERT(false, "unhandled instruction encountered: %d", instruction);
	}

	CUSTOM_ASSERT(false, "unknown instruction encountered: %d", instruction);
}

}}

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
		"\n  %d"
		"\n  %s"
		"\n  - type:     %s"
		"\n  - severity: %s"
		"\n  - source:   %s",
		id,
		message,
		type_string,
		severity_string,
		source_string
	);
}
#endif

static bool verify_compilation(GLuint id)
{
	GLint status = 0;
	glGetShaderiv(id, GL_COMPILE_STATUS, &status);
	if (status) { return true; }

	// @Note: linker will inform of the errors anyway
	GLint max_length = 0;
	glGetShaderiv(id, GL_INFO_LOG_LENGTH, &max_length);

	custom::Array<GLchar> info_log(max_length);
	glGetShaderInfoLog(id, max_length, &max_length, info_log.data);
	CUSTOM_MESSAGE("failed to compile shader:\n%s", info_log.data);

	return false;
}

static bool verify_linking(GLuint id)
{
	GLint status = 0;
	glGetProgramiv(id, GL_LINK_STATUS, &status);
	if (status) { return true; }

	GLint max_length = 0;
	glGetProgramiv(id, GL_INFO_LOG_LENGTH, &max_length);

	custom::Array<GLchar> info_log(max_length);
	glGetProgramInfoLog(id, max_length, &max_length, info_log.data);
	CUSTOM_MESSAGE("failed to link program:\n%s", info_log.data);

	return false;
}

static GLuint create_program(cstring source)
{
	// @Todo: read this meta info from outside?
	static Shader_Props compilations_props[] = {
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
