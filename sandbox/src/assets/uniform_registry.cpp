#include "engine/api/internal/types_names_lookup.h"
#include "engine/impl/array.h"

void init_uniform_names(void) {
	#define UNIFORM_IMPL(T) custom::uniform_names.push("u_" #T);
	#include "uniform_registry_impl.h"
}
