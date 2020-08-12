#include "engine/api/client/components_lookup.h"
#include "engine/impl/array.h"
#include "engine/impl/reference.h"
#include "engine/impl/entity_system.h"

#include "../components/types.h"

#define COMPONENT_IMPL(T)\
	/* @Note: initialize compile-time structs: */\
	template struct custom::Array<T>;\
	template struct custom::RefT<T>;\
	template custom::RefT<T> custom::Entity::add_component<T>(void);\
	template void custom::Entity::rem_component<T>(void);\
	template custom::RefT<T> custom::Entity::get_component<T>(void) const;\
	template bool custom::Entity::has_component<T>(void) const;\
	/* @Note: initialize compile-time statics: */\
	custom::Ref_Pool<T> custom::RefT<T>::pool;\
	u32 custom::Component_Registry<T>::type;\

#include "../components/registry_impl.h"

void init_entity_components(void) {
	// @Note: initialize runtime components' data:
	u32 component_types_count = 0;
	#define COMPONENT_IMPL(T) custom::Component_Registry<T>::type = component_types_count++;
	#include "../components/registry_impl.h"

	custom::component::count = component_types_count;
	custom::Entity::component_constructors.set_capacity(component_types_count);
	custom::Entity::component_destructors.set_capacity(component_types_count);
	custom::Entity::component_containers.set_capacity(component_types_count);
	#define COMPONENT_IMPL(T)\
		custom::Entity::component_constructors.push(&custom::ref_pool_create<T>);\
		custom::Entity::component_destructors.push(&custom::ref_pool_destroy<T>);\
		custom::Entity::component_containers.push(&custom::ref_pool_contains<T>);\

	#include "../components/registry_impl.h"
}
