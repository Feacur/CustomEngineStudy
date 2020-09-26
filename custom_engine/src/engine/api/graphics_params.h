#pragma once
#include "engine/core/types.h"
#include "engine/core/meta.h"
#include "engine/impl/math_bitwise.h"

namespace custom {
namespace graphics {

enum struct Clear_Flag : u8
{
	None    = 0,
	#define CLEAR_FLAG_IMPL(T, i) T = BIT(u8, i),
	#include "engine/registry_impl/clear_flag.h"
};
ENUM_FLAG_OPERATORS_IMPL(Clear_Flag)

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

enum struct Color_Write : u8
{
	None = 0,
	R    = BIT(u8, 0),
	G    = BIT(u8, 1),
	B    = BIT(u8, 2),
	A    = BIT(u8, 3),
};
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

enum struct Blend_Mode : u8
{
	Opaque,
	Alpha,
	Additive,
	Multiply,
};

enum struct Cull_Mode : u8
{
	None,
	Back,
	Front,
	Both,
};

enum struct Front_Face : u8
{
	CW,
	CCW,
};

enum struct Texture_Type : u8
{
	None,
	Color,
	Depth,
	DStencil,
	Stencil,
};

// @Note: final sampler type is implementation defined; OpenGL takes s32
typedef struct { u32 texture, sampler; } unit_id;

enum struct Data_Type : u8
{
	None,
	#define DATA_TYPE_IMPL(T) T,
	#include "engine/registry_impl/data_type.h"
};

enum struct Filter_Mode : u8
{
	None,
	Point,
	Linear,
};

enum struct Wrap_Mode : u8
{
	Repeat,
	Clamp,
	Mirror_Repeat,
	Mirror_Clamp,
};

enum struct Mesh_Frequency : u8
{
	Static,
	Dynamic,
	Stream,
};

enum struct Mesh_Access : u8
{
	Draw,
	Read,
	Copy,
};

enum struct Clip_Origin : u8
{
	Lower_Left,
	Upper_Left,
};

enum struct Clip_Depth : u8
{
	Neg_One,
	Zero_One,
};

enum struct Instruction : u8
{
	None,
	#define INSTRUCTION_IMPL(T) T,
	#include "engine/registry_impl/instruction.h"
};


}}
