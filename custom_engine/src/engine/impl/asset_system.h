#pragma once
#include "engine/api/internal/asset_system.h"

//
// assets
//

namespace custom {

template<typename T>
RefT<T> Assets::add(cstring id) {
	Ref const & ref = add(Asset_Registry<T>::type, id);
	return {ref.id, ref.gen};
}

template<typename T>
void Assets::rem(cstring id) {
	rem(Asset_Registry<T>::type, id);
}

template<typename T>
RefT<T> Assets::get(cstring id) {
	Ref const & ref = get(Asset_Registry<T>::type, id);
	return {ref.id, ref.gen};
}

template<typename T>
bool Assets::has(cstring id) {
	return has(Asset_Registry<T>::type, id);
}

template<typename T>
cstring Assets::get_path(RefT<T> const & ref) {
	return get_path(Asset_Registry<T>::type, ref);
}

}
