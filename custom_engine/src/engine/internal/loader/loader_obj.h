#include "obj_parser.h"

namespace custom {
namespace graphics {

template<typename T> constexpr static inline Data_Type get_data_type(void) { return Data_Type::None; }
#define GDT_IMPL(T) template<> constexpr inline Data_Type get_data_type<T>(void) { return Data_Type::T; }
	GDT_IMPL(unit_id);
	GDT_IMPL(s8); GDT_IMPL(s16); GDT_IMPL(s32);
	GDT_IMPL(u8); GDT_IMPL(u16); GDT_IMPL(u32);
	GDT_IMPL(r32); GDT_IMPL(r64);
	GDT_IMPL(vec2); GDT_IMPL(vec3); GDT_IMPL(vec4);
	GDT_IMPL(ivec2); GDT_IMPL(ivec3); GDT_IMPL(ivec4);
	GDT_IMPL(uvec2); GDT_IMPL(uvec3); GDT_IMPL(uvec4);
	GDT_IMPL(mat2); GDT_IMPL(mat3); GDT_IMPL(mat4);
#undef GDT_IMPL

}}

namespace custom {
namespace loader {

template<typename T>
static void write_data_array(custom::Array<T> const & data);

void mesh(u32 asset_id) {
	if (graphics::mark_pending_mesh(asset_id)) { return; }

	if (asset_id >= asset::mesh::count) { return; }
	cstring path = asset::mesh::paths[asset_id];

	Array<u8> file; file::read(path, file);
	if (file.count != file.capacity) { return; }

	Array<u8> vertex_attributes;
	Array<r32> vertices;
	Array<u32> indices;
	obj::parse(file, vertex_attributes, vertices, indices);

	u8 meta_id = asset::mesh::meta_ids[asset_id];
	asset::mesh::Meta const & meta = asset::mesh::meta_presets[meta_id];

	bc->write(graphics::Instruction::Allocate_Mesh);
	bc->write(asset_id);
	bc->write((u8)2);
	bc->write((b8)false); bc->write(graphics::Mesh_Frequency::Static); bc->write(graphics::Mesh_Access::Draw);
	bc->write(graphics::Data_Type::r32); bc->write(vertices.count); bc->write(vertices.count);
	bc->write_sized_array(vertex_attributes);
	bc->write((b8)true); bc->write(graphics::Mesh_Frequency::Static); bc->write(graphics::Mesh_Access::Draw);
	bc->write(graphics::Data_Type::u32); bc->write(indices.count); bc->write(indices.count);
	bc->write((u32)0);

	bc->write(graphics::Instruction::Load_Mesh);
	bc->write(asset_id);
	bc->write((u8)2);
	bc->write((u32)0); write_data_array(vertices);
	bc->write((u32)0); write_data_array(indices);
}

}}

//
// utilities
//

namespace custom {
namespace loader {

template<typename T>
static void write_data_array(custom::Array<T> const & data) {
	bc->write(graphics::get_data_type<T>());
	bc->write_sized_array(data);
}

}}
