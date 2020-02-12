#include "components.h"
#include "engine/impl/entity_system.h"

#define COMPONENT_IMPL(T)\
u32 T::offset;\
custom::Ref_Pool<T> T::pool;
#include "component_impl.h"

void init_entity_components(void) {
	u32 count = 0;

	#define COMPONENT_IMPL(T) T::offset = count++;
	#include "component_impl.h"

	custom::Entity::component_types_count = count;

	#define COMPONENT_IMPL(T) custom::World::component_pools.push(&T::pool);
	#include "component_impl.h"
}
