#pragma once
#include "engine/core/types.h"
#include "engine/core/meta.h"
#include "engine/impl/math_bitwise.h"

constexpr u32 const empty_asset_id = UINT32_MAX;

namespace custom {
namespace graphics {

enum struct Clear_Flags : u8
{
	None    = 0,
	Color   = BIT(u8, 0),
	Depth   = BIT(u8, 1),
	Stencil = BIT(u8, 2),
};
UNDERLYING_TYPE_META(Clear_Flags, u8)
IS_ENUM_META(Clear_Flags)
ENUM_FLAG_OPERATORS_IMPL(Clear_Flags)

enum struct Comparison : u8
{
	False,
	Less,
	LEqual,
	Equal,
	NEqual,
	GEqual,
	Greater,
	True,
};
UNDERLYING_TYPE_META(Comparison, u8)
IS_ENUM_META(Comparison)

enum struct Color_Write : u8
{
	None = 0,
	R    = BIT(u8, 0),
	G    = BIT(u8, 1),
	B    = BIT(u8, 2),
	A    = BIT(u8, 3),
};
UNDERLYING_TYPE_META(Color_Write, u8)
IS_ENUM_META(Color_Write)
ENUM_FLAG_OPERATORS_IMPL(Color_Write)

enum struct Operation : u8
{
	Keep,
	Zero,
	Replace,
	Incr,
	Incr_Wrap,
	Decr,
	Decr_Wrap,
	Invert,
};
UNDERLYING_TYPE_META(Operation, u8)
IS_ENUM_META(Operation)

enum struct Blend_Mode : u8
{
	Opaque,
	Alpha,
	Additive,
	Multiply,
};
UNDERLYING_TYPE_META(Blend_Mode, u8)
IS_ENUM_META(Blend_Mode)

enum struct Cull_Mode : u8
{
	None,
	Back,
	Front,
	Both,
};
UNDERLYING_TYPE_META(Cull_Mode, u8)
IS_ENUM_META(Cull_Mode)

enum struct Front_Face : u8
{
	CW,
	CCW,
};
UNDERLYING_TYPE_META(Front_Face, u8)
IS_ENUM_META(Front_Face)

enum struct Texture_Type : u8
{
	None,
	Color,
	Depth,
	DStencil,
	Stencil,
};
UNDERLYING_TYPE_META(Texture_Type, u8)
IS_ENUM_META(Texture_Type)

// @Note: final sampler type is implementation defined; OpenGL takes s32
typedef struct { u32 texture, sampler; } unit_id;
enum struct Data_Type : u8
{
	None,
	unit_id,
	s8, s16, s32,
	u8, u16, u32,
	r32, r64,
	vec2, vec3, vec4,
	ivec2, ivec3, ivec4,
	uvec2, uvec3, uvec4,
	mat2, mat3, mat4,
};
UNDERLYING_TYPE_META(Data_Type, u8)
IS_ENUM_META(Data_Type)

enum struct Filter_Mode : u8
{
	None,
	Point,
	Linear,
};
UNDERLYING_TYPE_META(Filter_Mode, u8)
IS_ENUM_META(Filter_Mode)

enum struct Wrap_Mode : u8
{
	Repeat,
	Clamp,
	Mirror_Repeat,
	Mirror_Clamp,
};
UNDERLYING_TYPE_META(Wrap_Mode, u8)
IS_ENUM_META(Wrap_Mode)

enum struct Shader_Part : u8
{
	None     = 0,
	Vertex   = BIT(u8, 0),
	Pixel    = BIT(u8, 1),
	Geometry = BIT(u8, 2),
	Compute  = BIT(u8, 3),
};
UNDERLYING_TYPE_META(Shader_Part, u8)
IS_ENUM_META(Shader_Part)
ENUM_FLAG_OPERATORS_IMPL(Shader_Part)

enum struct Mesh_Frequency : u8
{
	Static,
	Dynamic,
	Stream,
};
UNDERLYING_TYPE_META(Mesh_Frequency, u8)
IS_ENUM_META(Mesh_Frequency)

enum struct Mesh_Access : u8
{
	Draw,
	Read,
	Copy,
};
UNDERLYING_TYPE_META(Mesh_Access, u8)
IS_ENUM_META(Mesh_Access)

enum struct Clip_Origin : u8
{
	Lower_Left,
	Upper_Left,
};
UNDERLYING_TYPE_META(Clip_Origin, u8)
IS_ENUM_META(Clip_Origin)

enum struct Clip_Depth : u8
{
	NegOne_One,
	Zero_One,
};
UNDERLYING_TYPE_META(Clip_Depth, u8)
IS_ENUM_META(Clip_Depth)

enum struct Instruction : u8
{
	None,
	#define INSTRUCTION_IMPL(T) T,
	#include "instructions_registry_impl.h"
	Last,
};
UNDERLYING_TYPE_META(Instruction, u8)
IS_ENUM_META(Instruction)


}}
