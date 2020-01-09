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

	uint32 out = vector4_to_color32(color, image.offsets);
	
	for (int32 y = rect.bottom; y < rect.top; ++y) {
		uint32 *destination_line = image.data + y * image.size.x;
		for (int32 x = rect.left; x < rect.right; ++x) {
			uint32 *destination_uint32 = destination_line + x;
			*destination_uint32 = out;
		}
	}
}

void draw_rectangle(RGBA_Data image, Vector2 position, Vector2 size, Vector4 color) {
	Rendering_Rect rect = create_rendering_rect(position, size);
	rect = restrict_rendering_rect_to_image(rect, image.size);

	Vector4 source = color;
	
	for (int32 y = rect.bottom; y < rect.top; ++y) {
		uint32 *destination_line = image.data + y * image.size.x;
		for (int32 x = rect.left; x < rect.right; ++x) {
			uint32 *destination_uint32 = destination_line + x;
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
	
	for (int32 y = rect.bottom; y < rect.top; ++y) {
		float local_y = (float)y - position.y;
		uint32 *destination_line = image.data + y * image.size.x;
		for (int32 x = rect.left; x < rect.right; ++x) {
			float local_x = (float)x - position.x;
			
			MAKE_UV
			
			if (inside) {
				uint32 *destination_uint32 = destination_line + x;
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
	
	float texture_uv_width  = (float)(texture.size.x - 1);
	float texture_uv_height = (float)(texture.size.y - 1);
	
	for (int32 y = rect.bottom; y < rect.top; ++y) {
		float local_y = (float)y - position.y;
		uint32 *destination_line = image.data + y * image.size.x;
		for (int32 x = rect.left; x < rect.right; ++x) {
			float local_x = (float)x - position.x;
			
			MAKE_UV
			
			if (inside) {
				int32 texture_x  = (int32)(texture_uv_width * UV.x);
				int32 texture_y  = (int32)(texture_uv_height * UV.y);
				
				uint32 *source_uint32 = texture.data + texture_y * texture.size.x + texture_x;
				Vector4 source = color32_to_vector4(*source_uint32, texture.offsets);
				
				uint32 *destination_uint32 = destination_line + x;
				Vector4 destination = color32_to_vector4(*destination_uint32, image.offsets);
				
				Vector4 blend = interpolate(destination, source, source.w);
				*destination_uint32 = vector4_to_color32(blend, image.offsets);
			}
		}
	}
}


#undef CONSTANTS_AXES
#undef MAKE_UV
