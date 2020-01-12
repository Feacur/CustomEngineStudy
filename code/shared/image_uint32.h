struct RGBA_Data {
	u32   * data;
	Vector2i   size;
	u8      offsets[4];
};

inline void clear_buffer(RGBA_Data image, Vector4 color) {
	u32 rgba_color = vector4_to_color32(color, image.offsets);
	s32 pixels_count = image.size.x * image.size.y;
	for (s32 i = 0; i < pixels_count; ++i) {
		*(image.data + i) = rgba_color;
	}
}