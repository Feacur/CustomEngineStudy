#pragma once
#include "engine/api/internal/asset_system.h"

//
// asset
//

namespace custom {

template<typename T>
RefT<T> Asset::add(cstring id) {
	Ref const & ref = add(Asset_Registry<T>::type, id);
	return {ref.id, ref.gen};
}

template<typename T>
void Asset::rem(cstring id) {
	rem(Asset_Registry<T>::type, id);
}

template<typename T>
RefT<T> Asset::get(cstring id) {
	Ref const & ref = get(Asset_Registry<T>::type, id);
	return {ref.id, ref.gen};
}

template<typename T>
bool Asset::has(cstring id) {
	return has(Asset_Registry<T>::type, id);
}

template<typename T>
cstring Asset::get_path(RefT<T> const & ref) {
	return get_path(Asset_Registry<T>::type, (Asset const &)ref);
}

}
