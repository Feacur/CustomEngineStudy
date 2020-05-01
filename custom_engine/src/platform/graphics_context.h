#pragma once
#include "engine/core/types.h"

#define CUSTOM_WINDOW_CLASS_NAME "custom engine"

namespace custom {
namespace window {
struct Internal_Data; // @Note: an opaque pointer
}}

namespace custom {
namespace context {

struct Internal_Data; // @Note: an opaque pointer

Internal_Data * create(window::Internal_Data * window);
void destroy(Internal_Data * data);

void set_vsync(Internal_Data * data, s32 value);
bool check_vsync(Internal_Data * data);
void swap_buffers(Internal_Data * data);

}}
