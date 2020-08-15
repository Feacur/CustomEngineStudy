#pragma once
#include "engine/core/types.h"

namespace custom {
namespace component {

u32 count = 3;

#define COMPONENT_IMPL(T) #T,
cstring names[] = {
	#include "registry_impl.h"
	NULL
};

}}
