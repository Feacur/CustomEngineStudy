#include "components.h"
#include "engine/impl/entity_system.h"

#define COMPONENT_IMPL(T)\
template struct custom::Ref<T>;\
u32 T::offset;\
custom::Ref_Pool<T> T::pool;
#include "component_impl.h"

void init_entity_components(void) {
	#define COMPONENT_IMPL(T) T::offset = custom::Entity::component_types_count++;
	#include "component_impl.h"
	
	custom::Entity * e = &custom::Entity::pool.instances[0];
	#define COMPONENT_IMPL(T) {\
		e->add_component<T>();\
		custom::Ref<T> component_##T = e->get_component<T>();\
		bool has_##T = e->has_component<T>();\
		e->remove_component<T>();\
	}
	#include "component_impl.h"

	#define COMPONENT_IMPL(T) custom::World::component_pools.push(&T::pool);
	#include "component_impl.h"
}
