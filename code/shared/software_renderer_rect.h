struct Rendering_Rect {
	int32 left, right, bottom, top;
};

inline Rendering_Rect create_rendering_rect(Vector2 position, Vector2 size) {
	return {
		(int32)position.x, (int32)(position.x + size.x),
		(int32)position.y, (int32)(position.y + size.y)
	};
}

inline Rendering_Rect create_rendering_rect(Vector2 position, Vector2 size, Complex orientation) {
	Complex axis_x = orientation;
	Complex axis_y = {-axis_x.y, axis_x.x};
	
	Vector2 edge_x = axis_x * size.x;
	Vector2 edge_y = axis_y * size.y;
	
	float left   =  INFINITY;
	float bottom =  INFINITY;
	float right  = -INFINITY;
	float top    = -INFINITY;
	
	Vector2 local_vertices[] = {{0, 0}, edge_x, edge_y, edge_x + edge_y};
	int32 elements_in_local_vertices = C_ARRAY_LENGTH(local_vertices);
	for (int32 i = 0; i < elements_in_local_vertices; ++i) {
		Vector2 vertex = position + local_vertices[i];
		if (left   > vertex.x) { left   = vertex.x; }
		if (bottom > vertex.y) { bottom = vertex.y; }
		if (right  < vertex.x) { right  = vertex.x; }
		if (top    < vertex.y) { top    = vertex.y; }
	}
	
	return {
		(int32)left, (int32)right,
		(int32)bottom, (int32)top
	};
}

inline Rendering_Rect align_rendering_rect(Rendering_Rect rect, Vector2i size) {
	rect.left   = ((rect.left - 3) / 4) * 4;
	rect.right  = ((rect.right + 3) / 4) * 4;
	rect.bottom = rect.bottom - 1;
	rect.top    = rect.top + 1;
	
	return rect;
}

inline Rendering_Rect restrict_rendering_rect_to_image(Rendering_Rect rect, Vector2i size) {
	rect.left   = clamp(rect.left,   0, size.x);
	rect.right  = clamp(rect.right,  0, size.x);
	rect.bottom = clamp(rect.bottom, 0, size.y);
	rect.top    = clamp(rect.top,    0, size.y);
	
	return rect;
}
