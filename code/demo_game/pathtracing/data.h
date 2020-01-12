static u8 const checksum[8] = {'p', 'a', 't', 'h', 't', 'r', 'a', 'c'};

//
//
//

struct Hit_Data {
	s32 index;
	r32 distance;
};

struct Material {
	enum struct Type : u8 {
		Diffuse,
		Metal,
		Dielectric,
	};

	Type type;
	Vector3 albedo;
	Vector3 emission;
	r32 roughness;
	r32 refractive_index;
	
	Material & set_albedo(Vector3 value);
	Material & set_emission(Vector3 value);
	Material & set_roughness(r32 value);
	Material & set_refractive_index(r32 value);
};
UNDERLYING_TYPE_META(Material::Type, u8)
IS_ENUM_META(Material::Type)

struct Shape {
	enum struct Type : u8
	{
		Sphere,
		Aabb,
	};

	Type type;
	union {
		Aabb3   aabb;
		Vector4 sphere;
	};
};
UNDERLYING_TYPE_META(Shape::Type, u8)
IS_ENUM_META(Shape::Type)

struct Game_Data {
	u8 checksum[8];
	u32 random_state;
	Vector3 camera_position;
	Quaternion camera_rotation;
	Array_Dynamic<Shape> shapes;
	Array_Dynamic<Material> materials;
};

//
//
//

Material mat0(Material::Type type) { return { type, 0 }; }
Material & Material::set_albedo(Vector3 value) { albedo = value; return *this; }
Material & Material::set_emission(Vector3 value) { emission = value; return *this; }
Material & Material::set_roughness(r32 value) { roughness = value; return *this; }
Material & Material::set_refractive_index(r32 value) { refractive_index = value; return *this; }

Shape aabb(Aabb3 value) { Shape s = {Shape::Type::Aabb, 0}; s.aabb = value; return s; }
Shape sphere(Vector4 value) { Shape s = {Shape::Type::Sphere, 0}; s.sphere = value; return s; }

//
// funcitons
//

namespace camera {
	void reset_exposure(Game_Data * game_data) {
		if (!input::get_current(Pointer_Keys::Key1)) {
			return;
		}

		globals::render_buffer_f.exposure = 0;
		clear_buffer(globals::render_buffer_f, {0, 0, 0, 0});
	}

	void process_input_rotation(Game_Data * game_data) {
		if (!input::get_current(Pointer_Keys::Key1)) {
			return;
		}

		Vector2i pointer_delta = input::get_pointer_delta();
		game_data->camera_rotation = quaternion_multiply(
			game_data->camera_rotation,
			quaternion_from_radians(
				vec3((r32)-pointer_delta.y, (r32)pointer_delta.x, 0) * deg_to_rad
			)
		);
	}

	void process_input_position(Game_Data * game_data) {
		if (!input::get_current(Pointer_Keys::Key1)) {
			return;
		}

		r32 delta_time = globals::get_delta_seconds();

		r32 speed   = input::get_current(Keyboard_Keys::Shift) * 3 + 1.0f;
		
		r32 left    = input::get_current(Keyboard_Keys::A) * speed;
		r32 right   = input::get_current(Keyboard_Keys::D) * speed;
		r32 forward = input::get_current(Keyboard_Keys::W) * speed;
		r32 back    = input::get_current(Keyboard_Keys::S) * speed;
		r32 down    = input::get_current(Keyboard_Keys::Q) * speed;
		r32 up      = input::get_current(Keyboard_Keys::E) * speed;

		Vector3 offset = {right - left, up - down, forward - back};
		offset = quaternion_rotate_vector(game_data->camera_rotation, offset);
		game_data->camera_position = game_data->camera_position + offset * delta_time;
	}
}
