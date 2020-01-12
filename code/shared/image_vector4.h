struct Image_Data {
	Vector4  * data;
	Vector2i   size;
	r32      exposure;
};

inline void clear_buffer(Image_Data image, Vector4 color) {
	s32 pixels_count = image.size.x * image.size.y;
	for (s32 i = 0; i < pixels_count; ++i) {
		*(image.data + i) = color;
	}
}
