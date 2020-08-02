#pragma once
#include "engine/core/math_types.h"

namespace custom {
struct Bytecode;
}

namespace custom {
namespace renderer {

void init(Bytecode * bytecode);

void set_shader(u32 shader);
void set_mesh(u32 mesh);
void set_texture(u32 shader, u32 uniform, u32 texture);
void set_uniform(u32 shader, u32 uniform, mat4 const & matrix);
void set_uniform(u32 shader, u32 uniform, mat3 const & matrix);
void set_uniform(u32 shader, u32 uniform, ivec2 const & value);

void viewport(ivec2 const & position, ivec2 const & size);
void clear(void);
void draw(void);

}}
