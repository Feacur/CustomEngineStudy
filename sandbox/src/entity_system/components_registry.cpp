#include "engine/api/internal/names_lookup.h"
#include "engine/impl/array.h"
#include "engine/impl/reference.h"
#include "engine/impl/entity_system.h"

#include "component_types.h"

#define COMPONENT_IMPL(T)                                              \
template struct custom::Array<T>;                                      \
template struct custom::RefT<T>;                                       \
template custom::RefT<T> custom::Entity::add_component<T>(void);       \
template void custom::Entity::rem_component<T>(void);                  \
template custom::RefT<T> custom::Entity::get_component<T>(void) const; \
template bool custom::Entity::has_component<T>(void) const;            \

#include "../registry_impl/component_types.h"

namespace custom {

#define COMPONENT_IMPL(T)                                                           \
static REF_VOID_FUNC(ref_pool_create_##T) { return RefT<T>::pool.create(); }        \
static VOID_REF_FUNC(ref_pool_destroy_##T) { RefT<T>::pool.destroy(ref); }          \
static BOOL_REF_FUNC(ref_pool_contains_##T) { return RefT<T>::pool.contains(ref); } \

#include "../registry_impl/component_types.h"

}

namespace custom {

#define COMPONENT_IMPL(T)                       \
ENTITY_FROM_TO_FUNC(component_pool_copy_##T);   \
ENTITY_LOADING_FUNC(component_pool_load_##T);   \
ENTITY_LOADING_FUNC(component_pool_unload_##T); \

#include "../registry_impl/component_types.h"

}

namespace custom {
namespace serialization {

#define COMPONENT_IMPL(T) READ_FUNC(component_pool_read_##T);
#include "../registry_impl/component_types.h"

}}

void init_client_component_types(void) {
	#define COMPONENT_IMPL(T)                                                  \
	custom::Component_Registry<T>::type = custom::component_names.get_count(); \
	custom::component_names.store_string(#T, custom::empty_index);             \

	#include "../registry_impl/component_types.h"

	custom::Entity::vtable.create.set_capacity(custom::component_names.get_count());
	custom::Entity::vtable.destroy.set_capacity(custom::component_names.get_count());
	custom::Entity::vtable.contains.set_capacity(custom::component_names.get_count());
	custom::Entity::vtable.copy.set_capacity(custom::component_names.get_count());
	custom::Entity::vtable.load.set_capacity(custom::component_names.get_count());
	custom::Entity::vtable.unload.set_capacity(custom::component_names.get_count());
	custom::Entity::vtable.read.set_capacity(custom::component_names.get_count());

	#define COMPONENT_IMPL(T)                                                          \
	custom::Entity::vtable.create.push(&custom::ref_pool_create_##T);                  \
	custom::Entity::vtable.destroy.push(&custom::ref_pool_destroy_##T);                \
	custom::Entity::vtable.contains.push(&custom::ref_pool_contains_##T);              \
	custom::Entity::vtable.copy.push(&custom::component_pool_copy_##T);                \
	custom::Entity::vtable.load.push(&custom::component_pool_load_##T);                \
	custom::Entity::vtable.unload.push(&custom::component_pool_unload_##T);            \
	custom::Entity::vtable.read.push(&custom::serialization::component_pool_read_##T); \

	#include "../registry_impl/component_types.h"
}
