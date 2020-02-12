#pragma once
#include "engine/core/math_types.h"

namespace custom {
namespace window {

// @Note: an opaque data to be used as a pointer
typedef struct Internal_Data Data;

Data * create(void);
void destroy(Data * data);

void init_context(Data * data);

void update(Data * data);

void set_vsync(Data * data, s32 value);
bool is_vsync(Data * data);

void set_header(Data * data, cstring value);
ivec2 get_size(Data * data);
bool get_should_close(Data * data);

}}
