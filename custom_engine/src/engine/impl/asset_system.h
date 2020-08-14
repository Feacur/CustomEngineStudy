#pragma once
#include "engine/api/internal/asset_system.h"

//
// assets
//

namespace custom {

template<typename T>
RefT<T> Asset_System::add_asset(cstring id) {
	Ref const & ref = add_asset(Asset_Registry<T>::type, id);
	return {ref.id, ref.gen};
}

template<typename T>
void Asset_System::rem_asset(cstring id) {
	rem_asset(Asset_Registry<T>::type, id);
}

template<typename T>
RefT<T> Asset_System::get_asset(cstring id) {
	Ref const & ref = get_asset(Asset_Registry<T>::type, id);
	return {ref.id, ref.gen};
}

template<typename T>
bool Asset_System::has_asset(cstring id) {
	return has_asset(Asset_Registry<T>::type, id);
}

template<typename T>
cstring Asset_System::get_path(RefT<T> ref) {
	return get_path(Asset_Registry<T>::type, ref);
}

}
