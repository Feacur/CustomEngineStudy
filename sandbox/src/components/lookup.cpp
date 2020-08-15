#pragma once
#include "engine/core/code.h"
#include "engine/core/types.h"

namespace custom {
namespace component {

#define COMPONENT_IMPL(T) #T,
cstring names[] = {
	#include "registry_impl.h"
	NULL
};

u32 count = C_ARRAY_LENGTH(names) - 1;

}}
