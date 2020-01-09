constexpr inline float get_color32_component_as_float(uint32 color32, uint8 offset) {
	uint32 component = (color32 >> offset) & 0xff;
	return (float)component / 0xff;
}

constexpr inline Vector4 color32_to_vector4(uint32 color32, uint8 offsets[4]) {
	return {
		get_color32_component_as_float(color32, offsets[0]),
		get_color32_component_as_float(color32, offsets[1]),
		get_color32_component_as_float(color32, offsets[2]),
		get_color32_component_as_float(color32, offsets[3])
	};
}

constexpr inline uint32 make_color32_component_with_offset(float value, int32 offset) {
	uint32 component = (uint32)(value * 0xff) & 0xff;
	return component << offset;
}

constexpr inline uint32 vector4_to_color32(Vector4 color, uint8 offsets[4]) {
	return make_color32_component_with_offset(color.x, offsets[0])
		|  make_color32_component_with_offset(color.y, offsets[1])
		|  make_color32_component_with_offset(color.z, offsets[2])
		|  make_color32_component_with_offset(color.w, offsets[3]);
}
