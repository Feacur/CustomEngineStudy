#include "components.h"
#include "engine/impl/entity_system.h"

// @Note: instantiate component-specific templates and static data:
//        - entire Ref<T> structure
//        - static components' data
//        - entity-structure's methods
#define COMPONENT_IMPL(T)\
	template struct custom::Ref<T>;\
	u32 T::offset;\
	custom::Ref_Pool<T> T::pool;\
	template void custom::Entity::add_component<T>(void);\
	template custom::Ref<T> custom::Entity::get_component<T>(void);\
	template bool custom::Entity::has_component<T>(void) const;\
	template void custom::Entity::remove_component<T>(void);\

#include "components_registry_impl.h"

void init_entity_components(void) {
	// @Note: initialize runtime component-specific data
	//        - assign components' offsets
	//        - assign component pools
	u32 component_types_count = 0;
	#define COMPONENT_IMPL(T) T::offset = component_types_count++;
	#include "components_registry_impl.h"

	custom::Entity::component_pools.set_capacity(component_types_count);
	#define COMPONENT_IMPL(T) custom::Entity::component_pools.push(&T::pool);
	#include "components_registry_impl.h"
}
