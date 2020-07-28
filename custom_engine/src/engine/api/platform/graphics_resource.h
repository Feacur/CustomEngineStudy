#pragma once

#include "engine/core/types.h"

namespace custom {
namespace graphics {

bool is_inited_uniforms();

bool is_allocated_shader(u32 id);
bool is_allocated_texture(u32 id);
bool is_allocated_sampler(u32 id);
bool is_allocated_mesh(u32 id);
bool is_allocated_target(u32 id);

bool is_uploaded_shader(u32 id);
bool is_uploaded_texture(u32 id);
bool is_uploaded_mesh(u32 id);

}}
