#define CONSTANTS ;\
__m128 zero = _mm_set1_ps(0);\
__m128 one  = _mm_set1_ps(1);\
\
__m128 f255     = _mm_set1_ps(0xff);\
__m128i mask_ff = _mm_set1_epi32(0xff);\
\
__m128 position_x = _mm_set1_ps(position.x);\
__m128 position_y = _mm_set1_ps(position.y);


#define CONSTANTS_AXES ;\
Complex axis_x = orientation;\
Complex axis_y = {-axis_x.y, axis_x.x};\
axis_x = axis_x / size.x;\
axis_y = axis_y / size.y;\
\
__m128 axis_x_x = _mm_set1_ps(axis_x.x);\
__m128 axis_x_y = _mm_set1_ps(axis_x.y);\
__m128 axis_y_x = _mm_set1_ps(axis_y.x);\
__m128 axis_y_y = _mm_set1_ps(axis_y.y);


#define CONSTANTS_AXES_R ;\
__m128 axis_x_x = _mm_set1_ps(1 / size.x);\
__m128 axis_y_y = _mm_set1_ps(1 / size.y);


#define SEPARATE_CHANNELS(NAME, FROM) ;\
__m128 NAME##_r = _mm_cvtepi32_ps(_mm_and_si128(_mm_srli_epi32(NAME, FROM.offsets[0]), mask_ff));\
__m128 NAME##_g = _mm_cvtepi32_ps(_mm_and_si128(_mm_srli_epi32(NAME, FROM.offsets[1]), mask_ff));\
__m128 NAME##_b = _mm_cvtepi32_ps(_mm_and_si128(_mm_srli_epi32(NAME, FROM.offsets[2]), mask_ff));\
__m128 NAME##_a = _mm_cvtepi32_ps(_mm_and_si128(_mm_srli_epi32(NAME, FROM.offsets[3]), mask_ff));


#define MAKE_LOCAL_Y ;\
uint32 *destination_line = image.data + y * image.size.x;\
\
__m128 screen_y = _mm_set1_ps((float)y);\
__m128 local_y = _mm_sub_ps(screen_y, position_y);\


#define MAKE_LOCAL_X ;\
uint32 *destination_start = destination_line + x;\
\
__m128i destination = _mm_loadu_si128((__m128i *)destination_start);\
__m128 screen_x = _mm_set_ps((float)(x + 3), (float)(x + 2), (float)(x + 1), (float)(x + 0));\
__m128 local_x = _mm_sub_ps(screen_x, position_x);


#define MAKE_UV ;\
__m128 U = _mm_add_ps(_mm_mul_ps(local_x, axis_x_x), _mm_mul_ps(local_y, axis_x_y));\
__m128 V = _mm_add_ps(_mm_mul_ps(local_x, axis_y_x), _mm_mul_ps(local_y, axis_y_y));


#define MAKE_UV_R ;\
__m128 U = _mm_mul_ps(local_x, axis_x_x);\
__m128 V = _mm_mul_ps(local_y, axis_y_y);


#define SIMD_I(V, I) ((float *)&V)[I]
#define SIMDi_I(V, I) ((uint32 *)&V)[I]


#define READ_TEXTURE_COLOR ;\
__m128 texture_x = _mm_mul_ps(texture_uv_width,  _mm_min_ps(_mm_max_ps(U, zero), one));\
__m128 texture_y = _mm_mul_ps(texture_uv_height, _mm_min_ps(_mm_max_ps(V, zero), one));\
\
__m128i source = _mm_set1_epi32(0);\
for (int32 i = 0; i < 4; ++i) {\
	int32 texture_x_i = (int32)SIMD_I(texture_x, i);\
	int32 texture_y_i = (int32)SIMD_I(texture_y, i);\
	uint32 source_uint32 = *(texture.data + texture_y_i * texture.size.x + texture_x_i);\
	SIMDi_I(source, i) = source_uint32;\
}


#define ALPHA_BLEND ;\
__m128 blend_r = _mm_add_ps(destination_r, _mm_mul_ps(_mm_sub_ps(source_r, destination_r), blend_alpha));\
__m128 blend_g = _mm_add_ps(destination_g, _mm_mul_ps(_mm_sub_ps(source_g, destination_g), blend_alpha));\
__m128 blend_b = _mm_add_ps(destination_b, _mm_mul_ps(_mm_sub_ps(source_b, destination_b), blend_alpha));\
__m128 blend_a = _mm_add_ps(destination_a, _mm_mul_ps(_mm_sub_ps(source_a, destination_a), blend_alpha));\
\
__m128i blend_r_i = _mm_slli_epi32(_mm_cvtps_epi32(blend_r), image.offsets[0]);\
__m128i blend_g_i = _mm_slli_epi32(_mm_cvtps_epi32(blend_g), image.offsets[1]);\
__m128i blend_b_i = _mm_slli_epi32(_mm_cvtps_epi32(blend_b), image.offsets[2]);\
__m128i blend_a_i = _mm_slli_epi32(_mm_cvtps_epi32(blend_a), image.offsets[3]);\
\
__m128i out = _mm_or_si128(_mm_or_si128(_mm_or_si128(blend_r_i, blend_g_i), blend_b_i), blend_a_i);


#define WRITE_OUT_TO_DESTINATION ;\
__m128i inside_mask = _mm_castps_si128(_mm_and_ps(\
	_mm_and_ps(_mm_cmpge_ps(U, zero), _mm_cmplt_ps(U, one)),\
	_mm_and_ps(_mm_cmpge_ps(V, zero), _mm_cmplt_ps(V, one))\
));\
__m128i masked_out = _mm_or_si128(_mm_and_si128(inside_mask, out), _mm_andnot_si128(inside_mask, destination));\
_mm_storeu_si128((__m128i *)destination_start, masked_out);


void draw_rectangle_over(RGBA_Data image, Vector2 position, Vector2 size, Vector4 color) {
	Rendering_Rect rect = create_rendering_rect(position, size);
	rect = align_rendering_rect(rect, image.size);
	rect = restrict_rendering_rect_to_image(rect, image.size);

	CONSTANTS
	CONSTANTS_AXES_R
	
	uint32 source_uint32 = vector4_to_color32(color, image.offsets);
	__m128i out = _mm_set1_epi32(source_uint32);
	
	for (int32 y = rect.bottom; y < rect.top; ++y) {
		MAKE_LOCAL_Y
		for (int32 x = rect.left; x < rect.right; x += 4) {
			MAKE_LOCAL_X
			MAKE_UV_R
			WRITE_OUT_TO_DESTINATION
		}
	}
}

void draw_rectangle(RGBA_Data image, Vector2 position, Vector2 size, Vector4 color) {
	Rendering_Rect rect = create_rendering_rect(position, size);
	rect = align_rendering_rect(rect, image.size);
	rect = restrict_rendering_rect_to_image(rect, image.size);

	CONSTANTS
	CONSTANTS_AXES_R

	uint32 source_uint32 = vector4_to_color32(color, image.offsets);
	__m128i source = _mm_set1_epi32(source_uint32);
	SEPARATE_CHANNELS(source, image)
	__m128 blend_alpha = _mm_div_ps(source_a, f255);
	
	for (int32 y = rect.bottom; y < rect.top; ++y) {
		MAKE_LOCAL_Y
		for (int32 x = rect.left; x < rect.right; x += 4) {
			MAKE_LOCAL_X
			MAKE_UV_R
			SEPARATE_CHANNELS(destination, image)
			ALPHA_BLEND
			WRITE_OUT_TO_DESTINATION
		}
	}
}

void draw_rectangle(RGBA_Data image, Vector2 position, Vector2 size, Complex orientation, Vector4 color) {
	Rendering_Rect rect = create_rendering_rect(position, size, orientation);
	rect = align_rendering_rect(rect, image.size);
	rect = restrict_rendering_rect_to_image(rect, image.size);

	CONSTANTS
	CONSTANTS_AXES

	uint32 source_uint32 = vector4_to_color32(color, image.offsets);
	__m128i source = _mm_set1_epi32(source_uint32);
	SEPARATE_CHANNELS(source, image)
	__m128 blend_alpha = _mm_div_ps(source_a, f255);
	
	for (int32 y = rect.bottom; y < rect.top; ++y) {
		MAKE_LOCAL_Y
		for (int32 x = rect.left; x < rect.right; x += 4) {
			MAKE_LOCAL_X
			MAKE_UV
			SEPARATE_CHANNELS(destination, image)
			ALPHA_BLEND
			WRITE_OUT_TO_DESTINATION
		}
	}
}

void draw_rectangle(RGBA_Data image, Vector2 position, Vector2 size, Complex orientation, RGBA_Data texture) {
	Rendering_Rect rect = create_rendering_rect(position, size, orientation);
	rect = align_rendering_rect(rect, image.size);
	rect = restrict_rendering_rect_to_image(rect, image.size);

	CONSTANTS
	CONSTANTS_AXES
	
	__m128 texture_uv_width  = _mm_set1_ps((float)(texture.size.x - 1));
	__m128 texture_uv_height = _mm_set1_ps((float)(texture.size.y - 1));
	
	for (int32 y = rect.bottom; y < rect.top; ++y) {
		MAKE_LOCAL_Y
		for (int32 x = rect.left; x < rect.right; x += 4) {
			MAKE_LOCAL_X
			MAKE_UV
			READ_TEXTURE_COLOR
			SEPARATE_CHANNELS(source, texture)
			__m128 blend_alpha = _mm_div_ps(source_a, f255);
			SEPARATE_CHANNELS(destination, image)
			ALPHA_BLEND
			WRITE_OUT_TO_DESTINATION
		}
	}
}


#undef CONSTANTS
#undef CONSTANTS_AXES
#undef CONSTANTS_AXES_R
#undef SEPARATE_CHANNELS
#undef MAKE_LOCAL_Y
#undef MAKE_LOCAL_X
#undef MAKE_UV
#undef MAKE_UV_R
#undef ALPHA_BLEND
#undef WRITE_OUT_TO_DESTINATION
#undef SIMD_I
#undef SIMDi_I
#undef READ_TEXTURE_COLOR
