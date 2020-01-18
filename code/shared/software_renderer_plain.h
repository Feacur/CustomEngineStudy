#define CONSTANTS_AXES ;\
Complex axis_x = orientation;\
Complex axis_y = {-axis_x.y, axis_x.x};\
axis_x = axis_x / size.x;\
axis_y = axis_y / size.y;


#define MAKE_UV ;\
Vector2 local_position = {local_x, local_y};\
Vector2 UV = {\
	dot_product(local_position, axis_x),\
	dot_product(local_position, axis_y)\
};\
bool inside = (UV.x >= 0) && (UV.y >= 0) && (UV.x < 1) && (UV.y < 1);


void draw_rectangle_over(RGBA_Data image, Vector2 position, Vector2 size, Vector4 color) {
	Rendering_Rect rect = create_rendering_rect(position, size);
	rect = restrict_rendering_rect_to_image(rect, image.size);

	u32 out = vector4_to_color32(color, image.offsets);
	
	for (s32 y = rect.bottom; y < rect.top; ++y) {
		u32 *destination_line = image.data + y * image.size.x;
		for (s32 x = rect.left; x < rect.right; ++x) {
			u32 *destination_uint32 = destination_line + x;
			*destination_uint32 = out;
		}
	}
}

void draw_rectangle(RGBA_Data image, Vector2 position, Vector2 size, Vector4 color) {
	Rendering_Rect rect = create_rendering_rect(position, size);
	rect = restrict_rendering_rect_to_image(rect, image.size);

	Vector4 source = color;
	
	for (s32 y = rect.bottom; y < rect.top; ++y) {
		u32 *destination_line = image.data + y * image.size.x;
		for (s32 x = rect.left; x < rect.right; ++x) {
			u32 *destination_uint32 = destination_line + x;
			Vector4 destination = color32_to_vector4(*destination_uint32, image.offsets);
			
			Vector4 blend = interpolate(destination, source, source.w);
			*destination_uint32 = vector4_to_color32(blend, image.offsets);
		}
	}
}

void draw_rectangle(RGBA_Data image, Vector2 position, Vector2 size, Complex orientation, Vector4 color) {
	Rendering_Rect rect = create_rendering_rect(position, size, orientation);
	rect = restrict_rendering_rect_to_image(rect, image.size);

	CONSTANTS_AXES

	Vector4 source = color;
	
	for (s32 y = rect.bottom; y < rect.top; ++y) {
		r32 local_y = (r32)y - position.y;
		u32 *destination_line = image.data + y * image.size.x;
		for (s32 x = rect.left; x < rect.right; ++x) {
			r32 local_x = (r32)x - position.x;
			
			MAKE_UV
			
			if (inside) {
				u32 *destination_uint32 = destination_line + x;
				Vector4 destination = color32_to_vector4(*destination_uint32, image.offsets);
				
				Vector4 blend = interpolate(destination, source, source.w);
				*destination_uint32 = vector4_to_color32(blend, image.offsets);
			}
		}
	}
}

void draw_rectangle(RGBA_Data image, Vector2 position, Vector2 size, Complex orientation, RGBA_Data texture) {
	Rendering_Rect rect = create_rendering_rect(position, size, orientation);
	rect = restrict_rendering_rect_to_image(rect, image.size);
	
	CONSTANTS_AXES
	
	r32 texture_uv_width  = (r32)(texture.size.x - 1);
	r32 texture_uv_height = (r32)(texture.size.y - 1);
	
	for (s32 y = rect.bottom; y < rect.top; ++y) {
		r32 local_y = (r32)y - position.y;
		u32 *destination_line = image.data + y * image.size.x;
		for (s32 x = rect.left; x < rect.right; ++x) {
			r32 local_x = (r32)x - position.x;
			
			MAKE_UV
			
			if (inside) {
				s32 texture_x  = (s32)(texture_uv_width * UV.x);
				s32 texture_y  = (s32)(texture_uv_height * UV.y);
				
				u32 *source_uint32 = texture.data + texture_y * texture.size.x + texture_x;
				Vector4 source = color32_to_vector4(*source_uint32, texture.offsets);
				
				u32 *destination_uint32 = destination_line + x;
				Vector4 destination = color32_to_vector4(*destination_uint32, image.offsets);
				
				Vector4 blend = interpolate(destination, source, source.w);
				*destination_uint32 = vector4_to_color32(blend, image.offsets);
			}
		}
	}
}


#undef CONSTANTS_AXES
#undef MAKE_UV
