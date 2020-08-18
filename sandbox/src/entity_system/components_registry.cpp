#include "engine/api/internal/types_names_lookup.h"
#include "engine/impl/array.h"
#include "engine/impl/reference.h"
#include "engine/impl/entity_system.h"

#include "component_types.h"

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

#include "../registry_impl/component_types.h"

void init_client_component_types(void) {
	// @Note: initialize runtime components' data:
	#define COMPONENT_IMPL(T)\
		custom::Component_Registry<T>::type = custom::component_names.count;\
		custom::component_names.push(#T);\

	#include "../registry_impl/component_types.h"

	custom::Entity::component_constructors.set_capacity(custom::component_names.count);
	custom::Entity::component_destructors.set_capacity(custom::component_names.count);
	custom::Entity::component_containers.set_capacity(custom::component_names.count);
	#define COMPONENT_IMPL(T)\
		custom::Entity::component_constructors.push(&custom::ref_pool_create<T>);\
		custom::Entity::component_destructors.push(&custom::ref_pool_destroy<T>);\
		custom::Entity::component_containers.push(&custom::ref_pool_contains<T>);\

	#include "../registry_impl/component_types.h"
}
