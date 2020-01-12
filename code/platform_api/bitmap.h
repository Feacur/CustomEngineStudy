// From bottom left to top right for positive height
// From top left to bottom right for negative height

// Tightly packed structures
#pragma pack(push, 1)
struct Bitmap_File_Header {
	u8  file_type_1; // File type part, always 'B'
	u8  file_type_2; // File type part, always 'M'
	u32 file_size;   // Size of the file in bytes
	u16 reserved_1;  // Always 0
	u16 reserved_2;  // Always 0
	u32 data_offset; // Starting position of image data in bytes
};

struct Bitmap_Info_Header {
	// MARK: Windows 2.x       // Size = 12
	u32 this_size;          // Size of this header in bytes
	s32  width;              // Image width in pixels (Win 2.x had s16)
	s32  height;             // Image height in pixels (Win 2.x had s16)
	u16 planes;             // Number of color planes, which is 1 for BMP
	u16 bits_per_pixel;     // Number of bits per pixel
	// MARK: Windows 3.x       // Size = 40
	u32 compression;        // Compression methods used
	u32 size_of_bitmap;     // Size of bitmap in bytes
	s32  h_pixels_per_meter; // Horizontal resolution in pixels per meter
	s32  v_pixels_per_meter; // Vertical resolution in pixels per meter
	u32 colors_used;        // Number of colors in the image
	u32 colors_important;   // Minimum number of important colors
	// MARK: Windows 4.x       // Size = 108
	u32 red_mask;           // Mask identifying bits of red component
	u32 green_mask;         // Mask identifying bits of green component
	u32 blue_mask;          // Mask identifying bits of blue component
	u32 alpha_mask;         // Mask identifying bits of alpha component
};
#pragma pack(pop)

inline u8 get_mask_offset(u32 value) {
	u8 bits_in_value = 8 * sizeof(value);
	for (u8 i = 0; i < bits_in_value; ++i) {
		if (get_bit_at_index(value, i)) {
			return i;
		}
	}
	return 0;
}

RGBA_Data read_bitmap_image(Memory_Chunk *memory, u8 * bytes) {
	auto file_header = (Bitmap_File_Header *)bytes;
	if (file_header->file_type_1 != 'B') { return {}; }
	if (file_header->file_type_2 != 'M') { return {}; }
	
	auto info_header = (Bitmap_Info_Header *)(file_header + 1);
	if (info_header->planes != 1) { return {}; }
	if (info_header->compression != 3) { return {}; }
	
	s32 pixels_count = info_header->width * info_header->height;
	
	RGBA_Data result = {};
	result.size = {info_header->width, info_header->height};
	
	result.data = ALLOCATE_ARRAY(memory, u32, pixels_count);
	memcpy(result.data, bytes + file_header->data_offset, sizeof(u32) * pixels_count)
	
	result.offsets.red   = get_mask_offset(info_header->red_mask);
	result.offsets.green = get_mask_offset(info_header->green_mask);
	result.offsets.blue  = get_mask_offset(info_header->blue_mask);
	result.offsets.alpha = get_mask_offset(info_header->alpha_mask);
	
	return result;
}

RGBA_Data read_bitmap_file(cstring file_name, read_file_func * read_file) {
	u8 * bytes = read_file(&globals::transient_memory, file_name);
	if (bytes) {
		return read_bitmap_image(&globals::permanent_memory, bytes);
	}
	return {};
}
