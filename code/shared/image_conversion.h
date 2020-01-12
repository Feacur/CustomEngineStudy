constexpr inline r32 get_color32_component_as_float(u32 color32, u8 offset) {
	u32 component = (color32 >> offset) & 0xff;
	return (r32)component / 0xff;
}

constexpr inline Vector4 color32_to_vector4(u32 color32, u8 offsets[4]) {
	return {
		get_color32_component_as_float(color32, offsets[0]),
		get_color32_component_as_float(color32, offsets[1]),
		get_color32_component_as_float(color32, offsets[2]),
		get_color32_component_as_float(color32, offsets[3])
	};
}

constexpr inline u32 make_color32_component_with_offset(r32 value, s32 offset) {
	u32 component = (u32)(value * 0xff) & 0xff;
	return component << offset;
}

constexpr inline u32 vector4_to_color32(Vector4 color, u8 offsets[4]) {
	return make_color32_component_with_offset(color.x, offsets[0])
		|  make_color32_component_with_offset(color.y, offsets[1])
		|  make_color32_component_with_offset(color.z, offsets[2])
		|  make_color32_component_with_offset(color.w, offsets[3]);
}
