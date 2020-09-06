#include "engine/api/internal/names_lookup.h"
#include "engine/api/internal/component_types.h"
#include "engine/impl/array.h"
#include "engine/impl/reference.h"
#include "engine/impl/entity_system.h"

#define COMPONENT_IMPL(T)\
	/* @Note: initialize compile-time structs: */\
	template struct custom::Array<T>;\
	template struct custom::RefT<T>;\
	template custom::RefT<T> custom::Entity::add_component<T>(void);\
	template void custom::Entity::rem_component<T>(void);\
	template custom::RefT<T> custom::Entity::get_component<T>(void) const;\
	template bool custom::Entity::has_component<T>(void) const;\
	/* @Note: initialize compile-time statics: */\
	custom::Ref_PoolT<T> custom::RefT<T>::pool;\
	u32 custom::Component_Registry<T>::type;\

#include "engine/registry_impl/component_types.h"

void init_component_types(void) {
	// @Note: initialize runtime components' data:
	#define COMPONENT_IMPL(T)\
		custom::Component_Registry<T>::type = custom::component_names.get_count();\
		custom::component_names.store_string(#T, custom::empty_index);\

	#include "engine/registry_impl/component_types.h"

	custom::Entity::vtable.component_constructors.set_capacity(custom::component_names.get_count());
	custom::Entity::vtable.component_destructors.set_capacity(custom::component_names.get_count());
	custom::Entity::vtable.component_containers.set_capacity(custom::component_names.get_count());
	custom::Entity::vtable.component_copiers.set_capacity(custom::component_names.get_count());
	custom::Entity::vtable.component_loaders.set_capacity(custom::component_names.get_count());
	custom::Entity::vtable.component_unloaders.set_capacity(custom::component_names.get_count());
	custom::Entity::vtable.component_serialization_readers.set_capacity(custom::component_names.get_count());
	#define COMPONENT_IMPL(T)\
		custom::Entity::vtable.component_constructors.push(&custom::ref_pool_create<T>);\
		custom::Entity::vtable.component_destructors.push(&custom::ref_pool_destroy<T>);\
		custom::Entity::vtable.component_containers.push(&custom::ref_pool_contains<T>);\
		custom::Entity::vtable.component_copiers.push(&custom::component_pool_copy<T>);\
		custom::Entity::vtable.component_loaders.push(&custom::component_pool_load<T>);\
		custom::Entity::vtable.component_unloaders.push(&custom::component_pool_unload<T>);\
		custom::Entity::vtable.component_serialization_readers.push(&custom::serialization::component_pool_serialization_read<T>);\

	#include "engine/registry_impl/component_types.h"
}
