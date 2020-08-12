#pragma once
#include "engine/api/internal/entity_system.h"

// https://github.com/etodd/lasercrabs/blob/master/src/data/entity.h

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
