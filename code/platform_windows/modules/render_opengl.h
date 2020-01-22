#define FEATURE_OPENGL

// @Tode: use dll instead; that's why the lib is hardcoded here
#pragma comment(lib, "opengl32")

#include <gl/GL.h>

static GLuint texture_handle;

typedef BOOL WINAPI wglSwapIntervalEXT_func(int interval);
static wglSwapIntervalEXT_func * wglSwapIntervalEXT;

void verify_opengl_pixel_format(HDC device_context) {
	int active_format = GetPixelFormat(device_context);
	if (!active_format) {
		log_last_error();
		CUSTOM_ASSERT(false, "No active pixel format");
		return;
	}

	PIXELFORMATDESCRIPTOR pfd;
	int maximum_format = DescribePixelFormat(device_context, active_format, sizeof(pfd), &pfd);
	if (!maximum_format) {
		log_last_error();
		CUSTOM_ASSERT(false, "Can't describe pixel format");
		return;
	}

	if (!BITS_ARE_SET(pfd.dwFlags, PFD_SUPPORT_OPENGL)) {
		CUSTOM_ASSERT(false, "Pixel format doesn't support OpenGL");
		return;
	}
}

void init_opengl(HDC device_context) {
	PIXELFORMATDESCRIPTOR pfd = {};
	pfd.nSize        = sizeof(pfd);
	pfd.nVersion     = 1;
	pfd.dwFlags      = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType   = PFD_TYPE_RGBA;
	pfd.cColorBits   = (BYTE)(8 * 4);
	// pfd.cDepthBits   = (BYTE)(8 * 3);
	// pfd.cStencilBits = (BYTE)(8 * 1);
	
	int format = ChoosePixelFormat(device_context, &pfd);
	if (!format) {
		log_last_error();
		CUSTOM_ASSERT(false, "Can't choose pixel format");
		return;
	}
	
	if (!SetPixelFormat(device_context, format, &pfd)) {
		log_last_error();
		CUSTOM_ASSERT(false, "Can't set pixel format");
		return;
	}

	verify_opengl_pixel_format(device_context);

	HGLRC rendering_context = wglCreateContext(device_context);
	if (!rendering_context) {
		log_last_error();
		CUSTOM_ASSERT(false, "Can't create rendering context");
		return;
	}

	if (!wglMakeCurrent(device_context, rendering_context)) {
		log_last_error();
		CUSTOM_ASSERT(false, "Can't select rendering context");
		return;
	}

	CUSTOM_MESSAGE("vendor '%s'; renderer '%s'; version '%s'", glGetString(GL_VENDOR), glGetString(GL_RENDERER), glGetString(GL_VERSION));

	wglSwapIntervalEXT = (wglSwapIntervalEXT_func *)wglGetProcAddress("wglSwapIntervalEXT");
	if (wglSwapIntervalEXT) {
		wglSwapIntervalEXT(1);
	}
}

void display_render_buffer_opengl(HDC device_context) {
	glViewport(0, 0, window_size.x, window_size.y);

	glBindTexture(GL_TEXTURE_2D, texture_handle);
	glTexImage2D(
		GL_TEXTURE_2D, 0, GL_RGBA8,
		render_buffer.image.size.x, render_buffer.image.size.y,
		0, GL_BGRA_EXT, GL_UNSIGNED_BYTE,
		render_buffer.image.data
	);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glEnable(GL_TEXTURE_2D);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	Vector2 opengl_size = {
		(r32)render_buffer.destination_size.x / window_size.x,
		(r32)render_buffer.destination_size.y / window_size.y
	};
	glBegin(GL_TRIANGLES);
	glTexCoord2f(0, 0); glVertex2f(-opengl_size.x, -opengl_size.y);
	glTexCoord2f(1, 0); glVertex2f( opengl_size.x, -opengl_size.y);
	glTexCoord2f(1, 1); glVertex2f( opengl_size.x,  opengl_size.y);
	
	glTexCoord2f(0, 0); glVertex2f(-opengl_size.x, -opengl_size.y);
	glTexCoord2f(1, 1); glVertex2f( opengl_size.x,  opengl_size.y);
	glTexCoord2f(0, 1); glVertex2f(-opengl_size.x,  opengl_size.y);
	glEnd();

	SwapBuffers(device_context);
}
