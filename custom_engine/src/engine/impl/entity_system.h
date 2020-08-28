#pragma once
#include "engine/api/internal/entity_system.h"

//
// universal access
//

namespace custom {

template<typename T> FROM_TO_FUNC(ref_pool_copy) {
	*RefT<T>::pool.get_safe(to) = *RefT<T>::pool.get_safe(from);
}

}

namespace custom {
namespace serialization {

template<typename T> SERIALIZATION_READ_FUNC(component_pool_serialization_read);

}}

//
// entity
//

namespace custom {

template<typename T>
RefT<T> Entity::add_component(void) {
	return {add_component(Component_Registry<T>::type)};
}

template<typename T>
void Entity::rem_component(void) {
	rem_component(Component_Registry<T>::type);
}

template<typename T>
RefT<T> Entity::get_component(void) const {
	return {get_component(Component_Registry<T>::type)};
}

template<typename T>
bool Entity::has_component(void) const {
	return has_component(Component_Registry<T>::type);
}

}

//
// component ref
//

// @Note: experimental, mimics Asset_RefT; in case one needs fully self-contained ref
namespace custom {

template<typename T>
RefT<T> Component_RefT<T>::get(void) {
	return {Component{ref, entity, Component_Registry<T>::type}.get()};
}

template<typename T>
bool Component_RefT<T>::exists(void) const {
	return Component{ref, entity, Component_Registry<T>::type}.exists();
}

template<typename T>
void Component_RefT<T>::destroy(void) {
	Component{ref, entity, Component_Registry<T>::type}.destroy();
}

}
