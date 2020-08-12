#pragma once
#include "engine/api/internal/entity_system.h"
#include "engine/impl/array.h"

// https://github.com/etodd/lasercrabs/blob/master/src/data/entity.h

//
// entity components
//

namespace custom {

template<typename T> REF_VOID_FUNC(ref_pool_create) {
	return RefT<T>::pool.create();
}

template<typename T> BOOL_REF_FUNC(ref_pool_contains) {
	return RefT<T>::pool.contains(ref);
}

template<typename T> VOID_REF_FUNC(ref_pool_destroy) {
	if (RefT<T>::pool.contains(ref)) { RefT<T>::pool.destroy(ref); }
}

}

//
// entity
//

namespace custom {

template<typename T>
RefT<T> Entity::add_component(void) {
	Ref const & ref = add_component(Component_Registry<T>::type);
	return {ref.id, ref.gen};
}

template<typename T>
void Entity::rem_component(void) {
	rem_component(Component_Registry<T>::type);
}

template<typename T>
bool Entity::has_component(void) const {
	return has_component(Component_Registry<T>::type);
}

template<typename T>
RefT<T> Entity::get_component(void) const {
	Ref const & ref = get_component(Component_Registry<T>::type);
	return {ref.id, ref.gen};
}

}
