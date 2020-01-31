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

static GLint version_major;
static GLint version_minor;

namespace opengl {

struct Program
{
	GLuint id;
	custom::Array<GLuint> uniforms;
};
template struct custom::Array<Program>;

struct Texture
{
	GLuint id;
};
template struct custom::Array<Texture>;

struct Attribute
{
	u8 count;
};
template struct custom::Array<Attribute>;

struct Buffer
{
	GLuint id;
	custom::graphics::Data_Type type;
	u32 capacity, count;
	custom::Array<Attribute> attributes;
};
template struct custom::Array<Buffer>;

struct Mesh
{
	GLuint id;
	custom::Array<Buffer> buffers;
	u8 index_buffer;
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
	GLsizei length, glstring message, cmemory userParam
);
static bool link_program(GLuint program_id, cstring source, custom::graphics::Shader_Part parts);

namespace custom {
namespace graphics {

static void consume_single_instruction(Bytecode const & bc);

VM::VM()
{
	glGetIntegerv(GL_MAJOR_VERSION, &version_major);
	glGetIntegerv(GL_MINOR_VERSION, &version_minor);

	#if !defined(GES_SHIPPING)
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
		case Data_Type::tex: return GL_INT;
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
		CASE_IMPL(tex);
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
		CASE_IMPL(tex);
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

static cstring read_cstring(Bytecode const & bc) {
	u32 count = *bc.read<u32>();
	return bc.read<char>(count);
}

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
			ogl.programs.ensure_capacity(asset_id + 1);
			opengl::Program * resource = new (&ogl.programs[asset_id]) opengl::Program;
			resource->id = glCreateProgram();
		} return;

		case Instruction::Allocate_Texture: {
			u32   asset_id = *bc.read<u32>();
			ivec2 size = *bc.read<ivec2>();
			u8           channels     = *bc.read<u8>();
			Data_Type    data_type    = *bc.read<Data_Type>();
			Texture_Type texture_type = *bc.read<Texture_Type>();
			Filter_Mode  min_tex = *bc.read<Filter_Mode>();
			Filter_Mode  min_mip = *bc.read<Filter_Mode>();
			Filter_Mode  mag_tex = *bc.read<Filter_Mode>();
			Wrap_Mode    wrap_mode_x = *bc.read<Wrap_Mode>();
			Wrap_Mode    wrap_mode_y = *bc.read<Wrap_Mode>();

			ogl.textures.ensure_capacity(asset_id + 1);
			opengl::Texture * resource = new (&ogl.textures[asset_id]) opengl::Texture;

			// -- allocate memory --
			// if (version_major == 4 && version_minor >= 5 || version_major > 4) {
				glCreateTextures(GL_TEXTURE_2D, 1, &resource->id);
			// }
			// else {
			// 	glGenTextures(GL_TEXTURE_2D, 1, &resource->id);
			// }

			glTextureStorage2D(
				resource->id, 1,
				get_texture_internal_format(texture_type, data_type, channels),
				size.x, size.y
			);

			// -- chart memory --
			glTextureParameteri(resource->id, GL_TEXTURE_MIN_FILTER, get_min_filter(min_tex, min_mip));
			glTextureParameteri(resource->id, GL_TEXTURE_MAG_FILTER, get_mag_filter(mag_tex));
			glTextureParameteri(resource->id, GL_TEXTURE_WRAP_S, get_wrap_mode(wrap_mode_x));
			glTextureParameteri(resource->id, GL_TEXTURE_WRAP_T, get_wrap_mode(wrap_mode_y));
		} return;

		case Instruction::Allocate_Mesh: {
			u32 asset_id = *bc.read<u32>();
			ogl.meshes.ensure_capacity(asset_id + 1);
			opengl::Mesh * resource = new (&ogl.meshes[asset_id]) opengl::Mesh;

			u32 buffers_count = *bc.read<u32>();
			resource->buffers.set_capacity(buffers_count);
			for (u32 i = 0; i < buffers_count; ++i) {
				resource->buffers.push();
				opengl::Buffer * buffer = new (&resource->buffers[i]) opengl::Buffer;

				buffer->type = *bc.read<Data_Type>();
				buffer->capacity = *bc.read<u32>();

				u32 attr_count = *bc.read<u32>();
				buffer->attributes.set_capacity(attr_count);
				for (u32 attr_i = 0; attr_i < attr_count; ++attr_i) {
					buffer->attributes.push();
					opengl::Attribute * attribute = new (&buffer->attributes[attr_i]) opengl::Attribute;
					attribute->count = *bc.read<u8>();
				}
			}
			resource->index_buffer = *bc.read<u8>();

			// -- allocate memory --
			// if (version_major == 4 && version_minor >= 5 || version_major > 4) {
				for (u8 i = 0; i < resource->buffers.count; ++i) {
					opengl::Buffer & buffer = resource->buffers[i];
					glCreateBuffers(1, &buffer.id);
					glNamedBufferData(
						buffer.id,
						buffer.capacity * get_type_size(buffer.type),
						NULL, GL_STATIC_DRAW
					);
				}
			// }
			// else {
			// 	for (u8 i = 0; i < resource->buffers.count; ++i) {
			// 		opengl::Buffer & buffer = resource->buffers[i];
			// 		GLenum target = (i == resource->index_buffer) ? GL_ELEMENT_ARRAY_BUFFER : GL_ARRAY_BUFFER;
			// 		glGenBuffers(1, &buffer.id);
			// 		glBindBuffer(target, buffer.id);
			// 		glBufferData(
			// 			target,
			// 			buffer.capacity * get_type_size(buffer.type),
			// 			NULL, GL_STATIC_DRAW
			// 		);
			// 	}
			// }

			// -- chart memory --
			glGenVertexArrays(1, &resource->id);
			glBindVertexArray(resource->id);

			// if (version_major == 4 && version_minor >= 3 || version_major > 4) {
				for (u8 i = 0; i < resource->buffers.count; ++i) {
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
			// }
			// else {
			// 	for (u8 i = 0; i < resource->buffers.count; ++i) {
			// 		opengl::Buffer & buffer = resource->buffers[i];
			// 		u16 element_size = get_type_size(buffer.type);
			// 		GLenum element_type = get_data_type(buffer.type);
			// 
			// 		GLsizei stride = 0;
			// 		for (u8 attr_i = 0; attr_i < buffer.attributes.count; ++attr_i) {
			// 			opengl::Attribute & attr = buffer.attributes[attr_i];
			// 			stride += attr.count * element_size;
			// 		}
			// 
			// 		GLenum target = (i == resource->index_buffer) ? GL_ELEMENT_ARRAY_BUFFER : GL_ARRAY_BUFFER;
			// 		glBindBuffer(target, buffer.id);
			// 		uintptr_t attrib_offset = 0;
			// 		for (u8 attr_i = 0; attr_i < buffer.attributes.count; ++attr_i) {
			// 			opengl::Attribute & attr = buffer.attributes[attr_i];
			// 			glEnableVertexAttribArray(attr_i);
			// 			glVertexAttribPointer(
			// 				attr_i, attr.count, element_type, false,
			// 				stride, (cmemory)attrib_offset
			// 			);
			// 			attrib_offset += attr.count * element_size;
			// 		}
			// 	}
			// }

			
		} return;

		//
		case Instruction::Free_Shader: {
			u32 asset_id = *bc.read<u32>();
			opengl::Program const * resource = &ogl.programs[asset_id];
			glDeleteProgram(resource->id);
			resource->opengl::Program::~Program();
		} return;

		case Instruction::Free_Texture: {
			u32 asset_id = *bc.read<u32>();
			opengl::Texture const * resource = &ogl.textures[asset_id];
			glDeleteTextures(1, &resource->id);
			resource->opengl::Texture::~Texture();
		} return;

		case Instruction::Free_Mesh: {
			u32 asset_id = *bc.read<u32>();
			opengl::Mesh const * resource = &ogl.meshes[asset_id];
			for (u32 i = 0; i < resource->buffers.count; ++i) {
				GLenum target = (i == resource->index_buffer) ? GL_ELEMENT_ARRAY_BUFFER : GL_ARRAY_BUFFER;
				glDeleteBuffers(target, &resource->buffers[i].id);
			}
			glDeleteVertexArrays(1, &resource->id);
			resource->opengl::Mesh::~Mesh();
		} return;

		//
		case Instruction::Use_Shader: {
			u32 asset_id = *bc.read<u32>();
			opengl::Program const * resource = &ogl.programs[asset_id];
			glUseProgram(resource->id);
		} return;

		case Instruction::Use_Texture: {
			u32 asset_id = *bc.read<u32>();
			opengl::Texture const * resource = &ogl.textures[asset_id];
			s32 slot = *bc.read<s32>();
			glBindTextureUnit(slot, resource->id);
		} return;

		case Instruction::Use_Mesh: {
			u32 asset_id = *bc.read<u32>();
			opengl::Mesh const * resource = &ogl.meshes[asset_id];
			glBindVertexArray(resource->id);
			// glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, resource->indices.id);
		} return;

		//
		case Instruction::Load_Shader: {
			// @Change: receive a pointer instead, then free if needed?
			u32 asset_id = *bc.read<u32>();
			cstring source = read_cstring(bc);
			Shader_Part parts = *bc.read<Shader_Part>();

			opengl::Program * resource = &ogl.programs[asset_id];
			link_program(resource->id, source, parts);

			// @Todo: process uniforms
			// GLint uniform_location = glGetUniformLocation(id, uniform_name);
		} return;

		case Instruction::Load_Texture: {
			// @Change: receive a pointer instead, then free if needed?
			u32 asset_id = *bc.read<u32>();
			ivec2 size = *bc.read<ivec2>();
			u8           channels     = *bc.read<u8>();
			Data_Type    data_type    = *bc.read<Data_Type>();
			Texture_Type texture_type = *bc.read<Texture_Type>();
			cmemory data = read_data(bc, data_type, size.x * size.y * channels);

			opengl::Texture const * resource = &ogl.textures[asset_id];
			glTextureSubImage2D(
				resource->id,
				0,
				0, 0, size.x, size.y,
				get_texture_data_format(texture_type, channels),
				get_texture_data_type(texture_type, data_type),
				data
			);
		} return;
		
		case Instruction::Load_Mesh: {
			// @Change: receive a pointer instead, then free if needed?
			u32 asset_id = *bc.read<u32>();
			opengl::Mesh * resource = &ogl.meshes[asset_id];

			u32 buffers_count = *bc.read<u32>();
			for (u32 i = 0; i < buffers_count; ++i) {
				opengl::Buffer & buffer = resource->buffers[i];
				GLenum target = (i == resource->index_buffer) ? GL_ELEMENT_ARRAY_BUFFER : GL_ARRAY_BUFFER;

				DT_Array in_buffer = read_data_array(bc);
				// buffer.type  = in_buffer.type; // @Todo: should we check?
				buffer.count = in_buffer.count;

				glBindBuffer(target, buffer.id);
				glBufferData(
					target,
					in_buffer.count * get_type_size(in_buffer.type),
					in_buffer.data, GL_STATIC_DRAW
				);
			}
		} return;

		case Instruction::Load_Uniform: {
			// @Todo: automize location with some asset_id
			s32 location = *bc.read<s32>();
			DT_Array uniform = read_data_array(bc);

			switch (uniform.type) {
				case Data_Type::tex: glUniform1iv(location, uniform.count, (s32 *)uniform.data); break;
				//
				case Data_Type::r32:  glUniform1fv(location, uniform.count, (r32 *)uniform.data); break;
				case Data_Type::vec2: glUniform2fv(location, uniform.count, (r32 *)uniform.data); break;
				case Data_Type::vec3: glUniform3fv(location, uniform.count, (r32 *)uniform.data); break;
				case Data_Type::vec4: glUniform4fv(location, uniform.count, (r32 *)uniform.data); break;
				//
				case Data_Type::s32:   glUniform1iv(location, uniform.count, (s32 *)uniform.data); break;
				case Data_Type::ivec2: glUniform2iv(location, uniform.count, (s32 *)uniform.data); break;
				case Data_Type::ivec3: glUniform3iv(location, uniform.count, (s32 *)uniform.data); break;
				case Data_Type::ivec4: glUniform4iv(location, uniform.count, (s32 *)uniform.data); break;
				//
				case Data_Type::u32:   glUniform1uiv(location, uniform.count, (u32 *)uniform.data); break;
				case Data_Type::uvec2: glUniform2uiv(location, uniform.count, (u32 *)uniform.data); break;
				case Data_Type::uvec3: glUniform3uiv(location, uniform.count, (u32 *)uniform.data); break;
				case Data_Type::uvec4: glUniform4uiv(location, uniform.count, (u32 *)uniform.data); break;
				//
				case Data_Type::mat2: glUniformMatrix2fv(location, uniform.count, false, (r32 *)uniform.data); break;
				case Data_Type::mat3: glUniformMatrix3fv(location, uniform.count, false, (r32 *)uniform.data); break;
				case Data_Type::mat4: glUniformMatrix4fv(location, uniform.count, false, (r32 *)uniform.data); break;
			}
		} return;

		//
		case Instruction::Draw: {
			u32 asset_id = *bc.read<u32>();
			opengl::Mesh const * resource = &ogl.meshes[asset_id];
			opengl::Buffer const & indices = resource->buffers[resource->index_buffer];
			GLenum data_type = get_data_type(indices.type);
			// glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices.id);
			// // GLint program_id;
			// // glGetIntegerv(GL_CURRENT_PROGRAM, &program_id);
			// // glValidateProgram(program_id);
			glDrawElements(GL_TRIANGLES, indices.count, data_type, nullptr);
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
			cstring message = read_cstring(bc);
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

struct Shader_Props
{
	GLenum  type;
	cstring version;
	cstring defines;
};

static u8 fill_props(custom::graphics::Shader_Part parts, Shader_Props * props, u8 cap)
{
	u8 count = 0;

	if (bits_are_set(parts, custom::graphics::Shader_Part::Vertex)) {
		props[count++] = { GL_VERTEX_SHADER, "#version 330 core\n", "#define VERTEX_SECTION\n" };
	}

	if (bits_are_set(parts, custom::graphics::Shader_Part::Pixel)) {
		props[count++] = { GL_FRAGMENT_SHADER, "#version 330 core\n", "#define FRAGMENT_SECTION\n" };
	}

	if (bits_are_set(parts, custom::graphics::Shader_Part::Geometry)) {
		props[count++] = { GL_GEOMETRY_SHADER, "#version 330 core\n", "#define GEOMETRY_SECTION\n" };
	}

	if (bits_are_set(parts, custom::graphics::Shader_Part::Compute)) {
		props[count++] = { GL_COMPUTE_SHADER, "#version 430 core\n", "#define COMPUTE_SECTION\n" };
	}

	return count;
}

static bool link_program(GLuint program_id, cstring source, custom::graphics::Shader_Part parts)
{
	u8 const props_cap = 4;
	static Shader_Props props[props_cap];
	static GLuint       shaders[props_cap];
	u8 props_count = fill_props(parts, props, props_cap);

	// Compile shaders
	for (u8 i = 0; i < props_count; ++i) {
		glstring code[] = { props[i].version, props[i].defines, source };
		GLuint shader_id = glCreateShader(props[i].type);
		glShaderSource(shader_id, C_ARRAY_LENGTH(code), code, 0);
		glCompileShader(shader_id);
		shaders[i] = shader_id;
	}

	bool is_compiled = true;
	for (u8 i = 0; i < props_count; ++i) {
		bool isOk = verify_compilation(shaders[i]);
		is_compiled = is_compiled && isOk;
	}

	// Link the program
	for (u8 i = 0; i < props_count; ++i) {
		glAttachShader(program_id, shaders[i]);
	}
	glLinkProgram(program_id);
	bool is_linked = verify_linking(program_id);

	// Free shader resources
	for (u8 i = 0; i < props_count; ++i) {
		glDetachShader(program_id, shaders[i]);
	}
	for (u8 i = 0; i < props_count; ++i) {
		glDeleteShader(shaders[i]);
	}

	return is_compiled && is_linked;
}
