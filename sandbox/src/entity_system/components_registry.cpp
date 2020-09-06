#include "engine/api/internal/names_lookup.h"
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
	custom::Ref_PoolT<T> custom::RefT<T>::pool;\
	u32 custom::Component_Registry<T>::type;\

#include "../registry_impl/component_types.h"

void init_client_component_types(void) {
	// @Note: initialize runtime components' data:
	#define COMPONENT_IMPL(T)\
		custom::Component_Registry<T>::type = custom::component_names.get_count();\
		custom::component_names.store_string(#T, custom::empty_index);\

	#include "../registry_impl/component_types.h"

	custom::Entity::vtable.create.set_capacity(custom::component_names.get_count());
	custom::Entity::vtable.destroy.set_capacity(custom::component_names.get_count());
	custom::Entity::vtable.contains.set_capacity(custom::component_names.get_count());
	custom::Entity::vtable.copy.set_capacity(custom::component_names.get_count());
	custom::Entity::vtable.load.set_capacity(custom::component_names.get_count());
	custom::Entity::vtable.unload.set_capacity(custom::component_names.get_count());
	custom::Entity::vtable.serialization_read.set_capacity(custom::component_names.get_count());
	#define COMPONENT_IMPL(T)\
		custom::Entity::vtable.create.push(&custom::ref_pool_create<T>);\
		custom::Entity::vtable.destroy.push(&custom::ref_pool_destroy<T>);\
		custom::Entity::vtable.contains.push(&custom::ref_pool_contains<T>);\
		custom::Entity::vtable.copy.push(&custom::component_pool_copy<T>);\
		custom::Entity::vtable.load.push(&custom::component_pool_load<T>);\
		custom::Entity::vtable.unload.push(&custom::component_pool_unload<T>);\
		custom::Entity::vtable.serialization_read.push(&custom::serialization::component_pool_serialization_read<T>);\

	#include "../registry_impl/component_types.h"
}
