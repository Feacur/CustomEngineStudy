#pragma once
#include "engine/core/types.h"

#define CUSTOM_WINDOW_CLASS_NAME "custom engine"

namespace custom {
namespace window {
typedef struct Internal_Data Data;
}}

namespace custom {
namespace context {

typedef struct Internal_Data Data; // @Note: an opaque pointer

Data * create(window::Data * window);
void destroy(Data * data);

void set_vsync(Data * data, s32 value);
bool check_vsync(Data * data);
void swap_buffers(Data * data);

}}
