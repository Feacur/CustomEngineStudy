struct Render_Buffer {
	RGBA_Data  image;
	Image_Data image_f;
	BITMAPINFO info;
	//
	Vector2i   offset;
	int32      scaler_mul;
	int32      scaler_div;
	//
	Vector2i   source_size;
	Vector2i   destination_size;
};

static Render_Buffer render_buffer;

inline Vector2i get_window_size(HWND window) {
	RECT client_rect;
	GetClientRect(window, &client_rect);
	return {client_rect.right, client_rect.bottom};
}

inline Vector2i transform_window_to_render(Vector2i point) {
	return (point - render_buffer.offset)
		* render_buffer.scaler_mul
		/ render_buffer.scaler_div;
}

void make_render_buffer_fit(HWND window) {
	Vector2i source_size = render_buffer.source_size;
	
	if (render_settings.stretch_mode == Render_Settings::Stretch_Mode::None) {
		render_buffer.scaler_mul = 1;
		render_buffer.scaler_div = 1;
		render_buffer.destination_size = source_size;
	}
	else {
		int32 numerator_source = source_size.x * window_size.y;
		int32 numerator_window = window_size.x * source_size.y;

		if (numerator_source > numerator_window) {
			render_buffer.scaler_mul = source_size.x;
			render_buffer.scaler_div = window_size.x;
			render_buffer.destination_size = {
				window_size.x,
				numerator_window / source_size.x
			};
		}
		else {
			render_buffer.scaler_mul = source_size.y;
			render_buffer.scaler_div = window_size.y;
			render_buffer.destination_size = {
				numerator_source / source_size.y,
				window_size.y
			};
		}
	}
	
	render_buffer.offset = (window_size - render_buffer.destination_size) / 2;
}

void set_render_buffer_size(Vector2i size) {
	static int32 const bytes_per_pixel = sizeof(uint32);
	
	if (render_buffer.image.data) {
		free_memory(render_buffer.image.data);
	}
	
	if (render_buffer.image_f.data) {
		free_memory(render_buffer.image_f.data);
	}
	
	size.x = round_up_with_step(size.x, 4);
	size.y = round_up_with_step(size.y, 4);

	// Windows GDI expects ARGB image format
	render_buffer.image.offsets[0] = 16;
	render_buffer.image.offsets[1] =  8;
	render_buffer.image.offsets[2] =  0;
	render_buffer.image.offsets[3] = 24;
	
	render_buffer.image.data    = (uint32 *)allocate_memory(size.x * size.y * bytes_per_pixel);
	render_buffer.image.size    = size;
	ASSERT_TRUE(render_buffer.image.data, "Can't allocate render buffer image memory");

	render_buffer.image_f.data = (Vector4 *)allocate_memory(size.x * size.y * sizeof(Vector4));
	render_buffer.image_f.size = size;
	ASSERT_TRUE(render_buffer.image_f.data, "Can't allocate render buffer image memory");
	
	render_buffer.info.bmiHeader.biSize        = sizeof(render_buffer.info.bmiHeader);
	render_buffer.info.bmiHeader.biWidth       = size.x;
	render_buffer.info.bmiHeader.biHeight      = size.y;
	render_buffer.info.bmiHeader.biPlanes      = 1;
	render_buffer.info.bmiHeader.biBitCount    = (WORD)(8 * bytes_per_pixel);
	render_buffer.info.bmiHeader.biCompression = BI_RGB;
}

void display_blackness(HDC device_context) {
	BitBlt(
		// destination
		device_context,
		0, 0, window_size.x, window_size.y,
		// settings
		0, 0, 0,
		BLACKNESS
	);
}

void reinit_render_buffer(HWND window) {
	Vector2i size = render_settings.target_size;
	if (render_buffer.source_size == size) { return; }
	render_buffer.source_size = size;

	set_render_buffer_size(size);
	make_render_buffer_fit(window);

	HDC device_context = GetDC(window);
	display_blackness(device_context);

	SetStretchBltMode(device_context, STRETCH_DELETESCANS);
	// STRETCH_DELETESCANS: Deletes the pixels. This mode deletes all eliminated lines of pixels without trying to preserve their information.
	// STRETCH_HALFTONE:    Deletes the pixels. Maps pixels from the source rectangle into blocks of pixels in the destination rectangle. The average color over the destination block of pixels approximates the color of the source pixels.
	
	// After setting the STRETCH_HALFTONE stretching mode, an application must call the SetBrushOrgEx function to set the brush origin. If it fails to do so, brush misalignment occurs.
	// SetBrushOrgEx(device_context, 0, 0, NULL);

	LOG_TRACE("Reinitialized GDI parameters");
}

void display_render_buffer(HDC device_context) {
	auto buffer = render_buffer;
	StretchDIBits(
		// destination
		device_context,
		buffer.offset.x, buffer.offset.y,
		buffer.destination_size.x, buffer.destination_size.y,
		// source
		0, 0,
		buffer.image.size.x, buffer.image.size.y,
		buffer.image.data,
		// settings
		&buffer.info,
		DIB_RGB_COLORS, SRCCOPY
	);
}
