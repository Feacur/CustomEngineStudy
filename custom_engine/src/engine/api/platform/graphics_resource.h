#pragma once

#include "engine/core/types.h"

namespace custom {
namespace graphics {

bool has_inited_uniforms();

u32 mark_pending_uniforms();
u32 mark_pending_shader(u32 id);
u32 mark_pending_texture(u32 id);
u32 mark_pending_mesh(u32 id);
u32 mark_pending_target(u32 id);

}}
