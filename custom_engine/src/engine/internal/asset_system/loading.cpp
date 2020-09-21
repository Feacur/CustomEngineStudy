#include "custom_pch.h"

#include "engine/core/code.h"
#include "engine/core/collection_types.h"
#include "engine/core/math_types.h"
#include "engine/debug/log.h"
#include "engine/api/platform/file.h"
#include "engine/api/internal/names_lookup.h"
#include "engine/api/internal/asset_types.h"
#include "engine/impl/array.h"
#include "engine/impl/bytecode.h"
#include "engine/impl/asset_system.h"

namespace custom {

// @Todo: revisit
static void read_file_safely(cstring path, Array<u8> & buffer) {
	constexpr u32 count = 4;
	if (!file::get_time(path)) { CUSTOM_ASSERT(false, "file doesn't exist '%s'", path); return; }
	for (u32 i = 0; i < count; ++i) {
		if (file::read(path, buffer)) { return; }
	}
	CUSTOM_ASSERT(false, "failed to read file safely: '%s'", path);
}

}

//
//
//

namespace custom {
namespace loader {

static Bytecode * bc = NULL;

void init(Bytecode * bytecode) {
	bc = bytecode;
}

}}

// namespace custom {
// namespace graphics {
// 
// template<typename T> constexpr static inline Data_Type get_data_type(void) { return Data_Type::None; }
// #define DATA_TYPE_IMPL(T) template<> constexpr inline Data_Type get_data_type<T>(void) { return Data_Type::T; }
// #include "engine/registry_impl/data_type.h"
// 
// }}

//
// Shader_Asset
//

namespace custom {
namespace loading {

template<> LOADING_FUNC(asset_pool_load<Shader_Asset>) {
	if (!asset_ref.exists()) { CUSTOM_ASSERT(false, "shader asset doesn't exist"); return; }

	RefT<Shader_Asset> & refT = (RefT<Shader_Asset> &)asset_ref;
	if (!refT.exists()) { CUSTOM_ASSERT(false, "asset doesn exist"); }

	//
	cstring path = asset_ref.get_path();
	Array<u8> file; read_file_safely(path, file);
	if (!file.count) { return; }

	Shader_Asset * asset = refT.get_fast();
	asset->update(file);

	// @Note: direct asset to the GVM
	custom::loader::bc->write(graphics::Instruction::Allocate_Shader);
	custom::loader::bc->write((Ref &)asset_ref);
	
	custom::loader::bc->write(graphics::Instruction::Load_Shader);
	custom::loader::bc->write((Ref &)asset_ref);
}

template<> LOADING_FUNC(asset_pool_unload<Shader_Asset>) {
	if (!asset_ref.exists()) { CUSTOM_ASSERT(false, "shader asset doesn't exist"); return; }

	RefT<Shader_Asset> & refT = (RefT<Shader_Asset> &)asset_ref;
	if (!refT.exists()) { CUSTOM_ASSERT(false, "asset doesn exist"); }

	//
	Shader_Asset * asset = refT.get_fast();
	asset->~Shader_Asset();

	// @Note: remove asset from the GVM
	custom::loader::bc->write(graphics::Instruction::Free_Shader);
	custom::loader::bc->write((Ref &)asset_ref);
}

template<> LOADING_FUNC(asset_pool_update<Shader_Asset>) {
	if (!asset_ref.exists()) { CUSTOM_ASSERT(false, "shader asset doesn't exist"); return; }

	RefT<Shader_Asset> & refT = (RefT<Shader_Asset> &)asset_ref;
	if (!refT.exists()) { CUSTOM_ASSERT(false, "asset doesn exist"); }

	//
	cstring path = asset_ref.get_path();
	Array<u8> file; read_file_safely(path, file);
	if (!file.count) { return; }

	Shader_Asset * asset = refT.get_fast();
	asset->update(file);

	// @Note: direct asset to the GVM
	custom::loader::bc->write(graphics::Instruction::Free_Shader);
	custom::loader::bc->write((Ref &)asset_ref);

	custom::loader::bc->write(graphics::Instruction::Allocate_Shader);
	custom::loader::bc->write((Ref &)asset_ref);
	
	custom::loader::bc->write(graphics::Instruction::Load_Shader);
	custom::loader::bc->write((Ref &)asset_ref);
}

}}

//
// Texture_Asset
//

namespace custom {
namespace loading {

template<> LOADING_FUNC(asset_pool_load<Texture_Asset>) {
	if (!asset_ref.exists()) { CUSTOM_ASSERT(false, "texture asset doesn't exist"); return; }

	RefT<Texture_Asset> & refT = (RefT<Texture_Asset> &)asset_ref;
	if (!refT.exists()) { CUSTOM_ASSERT(false, "asset doesn exist"); }

	//
	cstring path = asset_ref.get_path();
	Array<u8> file; read_file_safely(path, file);
	if (!file.count) { return; }

	Texture_Asset * asset = refT.get_fast();
	asset->update(file);

	// @Note: direct asset to the GVM
	custom::loader::bc->write(graphics::Instruction::Allocate_Texture);
	custom::loader::bc->write((Ref &)asset_ref);

	custom::loader::bc->write(graphics::Instruction::Load_Texture);
	custom::loader::bc->write((Ref &)asset_ref);
}

template<> LOADING_FUNC(asset_pool_unload<Texture_Asset>) {
	if (!asset_ref.exists()) { CUSTOM_ASSERT(false, "texture asset doesn't exist"); return; }

	RefT<Texture_Asset> & refT = (RefT<Texture_Asset> &)asset_ref;
	if (!refT.exists()) { CUSTOM_ASSERT(false, "asset doesn exist"); }

	//
	Texture_Asset * asset = refT.get_fast();
	asset->~Texture_Asset();

	// @Note: remove asset from the GVM
	custom::loader::bc->write(graphics::Instruction::Free_Texture);
	custom::loader::bc->write((Ref &)asset_ref);
}

template<> LOADING_FUNC(asset_pool_update<Texture_Asset>) {
	if (!asset_ref.exists()) { CUSTOM_ASSERT(false, "texture asset doesn't exist"); return; }

	RefT<Texture_Asset> & refT = (RefT<Texture_Asset> &)asset_ref;
	if (!refT.exists()) { CUSTOM_ASSERT(false, "asset doesn exist"); }

	//
	cstring path = asset_ref.get_path();
	Array<u8> file; read_file_safely(path, file);
	if (!file.count) { return; }

	Texture_Asset * asset = refT.get_fast();
	asset->update(file);

	// @Note: direct asset to the GVM
	custom::loader::bc->write(graphics::Instruction::Free_Texture);
	custom::loader::bc->write((Ref &)asset_ref);

	custom::loader::bc->write(graphics::Instruction::Allocate_Texture);
	custom::loader::bc->write((Ref &)asset_ref);

	custom::loader::bc->write(graphics::Instruction::Load_Texture);
	custom::loader::bc->write((Ref &)asset_ref);
}

}}

//
// Mesh_Asset
//

namespace custom {
namespace loading {

template<> LOADING_FUNC(asset_pool_load<Mesh_Asset>) {
	if (!asset_ref.exists()) { CUSTOM_ASSERT(false, "mesh asset doesn't exist"); return; }

	RefT<Mesh_Asset> & refT = (RefT<Mesh_Asset> &)asset_ref;
	if (!refT.exists()) { CUSTOM_ASSERT(false, "asset doesn exist"); }

	//
	cstring path = asset_ref.get_path();
	Array<u8> file; read_file_safely(path, file);
	if (!file.count) { return; }

	Mesh_Asset * asset = refT.get_fast();
	asset->buffers.data     = NULL;
	asset->buffers.capacity = 0;
	asset->buffers.count    = 0;
	asset->update(file);

	// @Note: direct asset to the GVM
	custom::loader::bc->write(graphics::Instruction::Allocate_Mesh);
	custom::loader::bc->write((Ref &)asset_ref);

	custom::loader::bc->write(graphics::Instruction::Load_Mesh);
	custom::loader::bc->write((Ref &)asset_ref);
}

template<> LOADING_FUNC(asset_pool_unload<Mesh_Asset>) {
	if (!asset_ref.exists()) { CUSTOM_ASSERT(false, "mesh asset doesn't exist"); return; }

	RefT<Mesh_Asset> & refT = (RefT<Mesh_Asset> &)asset_ref;
	if (!refT.exists()) { CUSTOM_ASSERT(false, "asset doesn exist"); }

	//
	Mesh_Asset * asset = refT.get_fast();
	asset->~Mesh_Asset();

	// @Note: remove asset from the GVM
	custom::loader::bc->write(graphics::Instruction::Free_Mesh);
	custom::loader::bc->write((Ref &)asset_ref);
}

template<> LOADING_FUNC(asset_pool_update<Mesh_Asset>) {
	if (!asset_ref.exists()) { CUSTOM_ASSERT(false, "mesh asset doesn't exist"); return; }

	RefT<Mesh_Asset> & refT = (RefT<Mesh_Asset> &)asset_ref;
	if (!refT.exists()) { CUSTOM_ASSERT(false, "asset doesn exist"); }

	//
	cstring path = asset_ref.get_path();
	Array<u8> file; read_file_safely(path, file);
	if (!file.count) { return; }

	Mesh_Asset * asset = refT.get_fast();
	asset->update(file);

	// @Note: direct asset to the GVM
	custom::loader::bc->write(graphics::Instruction::Free_Mesh);
	custom::loader::bc->write((Ref &)asset_ref);

	custom::loader::bc->write(graphics::Instruction::Allocate_Mesh);
	custom::loader::bc->write((Ref &)asset_ref);

	custom::loader::bc->write(graphics::Instruction::Load_Mesh);
	custom::loader::bc->write((Ref &)asset_ref);
}

}}

//
// Collider2d_Asset
//

namespace custom {
namespace loading {

template<> LOADING_FUNC(asset_pool_load<Collider2d_Asset>) {
	if (!asset_ref.exists()) { CUSTOM_ASSERT(false, "mesh asset doesn't exist"); return; }

	RefT<Collider2d_Asset> & refT = (RefT<Collider2d_Asset> &)asset_ref;
	if (!refT.exists()) { CUSTOM_ASSERT(false, "asset doesn exist"); }

	//
	cstring path = asset_ref.get_path();
	Array<u8> file; read_file_safely(path, file);
	if (!file.count) { return; }

	Collider2d_Asset * asset = refT.get_fast();
	asset->points.data     = NULL;
	asset->points.capacity = 0;
	asset->points.count    = 0;
	asset->update(file);
}

template<> LOADING_FUNC(asset_pool_unload<Collider2d_Asset>) {
	if (!asset_ref.exists()) { CUSTOM_ASSERT(false, "mesh asset doesn't exist"); return; }

	RefT<Collider2d_Asset> & refT = (RefT<Collider2d_Asset> &)asset_ref;
	if (!refT.exists()) { CUSTOM_ASSERT(false, "asset doesn exist"); }

	//
	Collider2d_Asset * asset = refT.get_fast();
	asset->~Collider2d_Asset();
}

template<> LOADING_FUNC(asset_pool_update<Collider2d_Asset>) {
	if (!asset_ref.exists()) { CUSTOM_ASSERT(false, "mesh asset doesn't exist"); return; }

	RefT<Collider2d_Asset> & refT = (RefT<Collider2d_Asset> &)asset_ref;
	if (!refT.exists()) { CUSTOM_ASSERT(false, "asset doesn exist"); }

	//
	cstring path = asset_ref.get_path();
	Array<u8> file; read_file_safely(path, file);
	if (!file.count) { return; }

	Collider2d_Asset * asset = refT.get_fast();
	asset->update(file);
}

}}

//
// Prefab_Asset
//

namespace custom {
namespace loading {

Entity entity_read(Array<u8> & file) {
	file.push('\0'); --file.count;

	cstring source = (cstring)file.data;
	Entity entity = Entity::create(false);
	entity.read(&source);

	return entity;
	// @Note: some weird behaviour occured here, 29 August 2020;
	//       optimization related or memory related or something else, I don't grasp currently?
	//       loading partially and sporadically fails if you first store
	//       the asset pointer and immediately assign it with `*asset = {prefab}`;
	//       everything's fine, however if you do as it's done above
	//       the notorious printf()-"fix" works, too
	//       ...
	//       although, if Ref is inherited, everything seems to bee alright
	//       just be aware
}

template<> LOADING_FUNC(asset_pool_load<Prefab_Asset>) {
	if (!asset_ref.exists()) { CUSTOM_ASSERT(false, "prefab asset doesn't exist"); return; }

	RefT<Prefab_Asset> & refT = (RefT<Prefab_Asset> &)asset_ref;
	if (!refT.exists()) { CUSTOM_ASSERT(false, "asset doesn exist"); }

	//
	cstring path = asset_ref.get_path();
	Array<u8> file; read_file_safely(path, file);
	if (!file.count) { return; }

	Prefab_Asset * asset = refT.get_fast();
	Entity prefab_entity = entity_read(file);

	// @Note: memory might have been relocated
	if (!refT.exists()) { CUSTOM_ASSERT(false, "asset doesn exist"); }
	asset = refT.get_fast();
	asset->entity = prefab_entity;
}

template<> LOADING_FUNC(asset_pool_unload<Prefab_Asset>) {
	if (!asset_ref.exists()) { CUSTOM_ASSERT(false, "prefab asset doesn't exist"); return; }

	RefT<Prefab_Asset> & refT = (RefT<Prefab_Asset> &)asset_ref;
	if (!refT.exists()) { CUSTOM_ASSERT(false, "asset doesn exist"); }

	//
	Prefab_Asset * asset = refT.get_fast();
	Entity prefab_entity = asset->entity;
	asset->entity = {custom::empty_ref};

	prefab_entity.destroy();
}

template<> LOADING_FUNC(asset_pool_update<Prefab_Asset>) {
	if (!asset_ref.exists()) { CUSTOM_ASSERT(false, "prefab asset doesn't exist"); return; }

	RefT<Prefab_Asset> & refT = (RefT<Prefab_Asset> &)asset_ref;
	if (!refT.exists()) { CUSTOM_ASSERT(false, "asset doesn exist"); }

	//
	cstring path = asset_ref.get_path();
	Array<u8> file; read_file_safely(path, file);
	if (!file.count) { return; }

	// @Todo: update descendants, too?
	Prefab_Asset * asset = refT.get_fast();
	asset->entity.destroy();

	// @Note: memory might have been relocated
	if (!refT.exists()) { CUSTOM_ASSERT(false, "asset doesn exist"); }
	asset = refT.get_fast();
	Entity prefab_entity = entity_read(file);

	// @Note: memory might have been relocated
	if (!refT.exists()) { CUSTOM_ASSERT(false, "asset doesn exist"); }
	asset = refT.get_fast();
	asset->entity = prefab_entity;
}

}}

//
// Config_Asset
//

namespace custom {
namespace loading {

template<> LOADING_FUNC(asset_pool_load<Config_Asset>) {
	if (!asset_ref.exists()) { CUSTOM_ASSERT(false, "prefab asset doesn't exist"); return; }

	RefT<Config_Asset> & refT = (RefT<Config_Asset> &)asset_ref;
	if (!refT.exists()) { CUSTOM_ASSERT(false, "asset doesn exist"); }

	//
	cstring path = asset_ref.get_path();
	Array<u8> file; read_file_safely(path, file);
	if (!file.count) { return; }

	Config_Asset * asset = refT.get_fast();
	asset->entries.data     = NULL;
	asset->entries.capacity = 0;
	asset->entries.count    = 0;
	asset->update(file);

	// @Note: config is a passive data storage
}

template<> LOADING_FUNC(asset_pool_unload<Config_Asset>) {
	if (!asset_ref.exists()) { CUSTOM_ASSERT(false, "config asset doesn't exist"); return; }

	RefT<Config_Asset> & refT = (RefT<Config_Asset> &)asset_ref;
	if (!refT.exists()) { CUSTOM_ASSERT(false, "asset doesn exist"); }

	//
	Config_Asset * asset = refT.get_fast();
	asset->entries.~Array();
}

template<> LOADING_FUNC(asset_pool_update<Config_Asset>) {
	if (!asset_ref.exists()) { CUSTOM_ASSERT(false, "config asset doesn't exist"); return; }

	RefT<Config_Asset> & refT = (RefT<Config_Asset> &)asset_ref;
	if (!refT.exists()) { CUSTOM_ASSERT(false, "asset doesn exist"); }

	//
	cstring path = asset_ref.get_path();
	Array<u8> file; read_file_safely(path, file);
	if (!file.count) { return; }

	Config_Asset * asset = refT.get_fast();
	asset->update(file);

	// @Note: config is a passive data storage
}

}}
