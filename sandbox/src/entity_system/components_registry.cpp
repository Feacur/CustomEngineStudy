#include "engine/impl/entity_system.h"

#include "components.h"

// @Note: initialize compile-time components' data:
#define COMPONENT_IMPL(T)\
	template struct custom::RefT<T>;\
	custom::Ref_Pool<T> custom::RefT<T>::pool;\
	u32 custom::Component_Registry<T>::offset;\
	template custom::RefT<T> custom::Entity::add_component<T>(void);\
	template custom::RefT<T> custom::Entity::add_component<T, T const &>(T const & data);\
	template void custom::Entity::remove_component<T>(void);\
	template custom::RefT<T> custom::Entity::get_component<T>(void) const;\
	template bool custom::Entity::has_component<T>(void) const;\

#include "components_registry_impl.h"

void init_entity_components(void) {
	// @Note: initialize runtime components' data:
	u32 component_types_count = 0;
	#define COMPONENT_IMPL(T) custom::Component_Registry<T>::offset = component_types_count++;
	#include "components_registry_impl.h"

	custom::Entity::component_destructors.set_capacity(component_types_count);
	#define COMPONENT_IMPL(T) custom::Entity::component_destructors.push(&custom::ref_pool_destruct<T>);
	#include "components_registry_impl.h"
}
