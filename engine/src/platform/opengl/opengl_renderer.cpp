#include "custom_pch.h"
#include "engine/debug/log.h"
#include "engine/platform/opengl_renderer.h"

#include <glad/glad.h>

// https://www.khronos.org/registry/OpenGL/index_gl.php

// https://github.com/etodd/lasercrabs/blob/master/src/platform/glvm.cpp

static void opengl_message_callback(
	unsigned source, unsigned type, unsigned id, unsigned severity,
	int length, cstring message, void const * userParam
);

//
// API implementation
//

namespace custom
{
	Opengl_Renderer::Opengl_Renderer()
	{
		#if !defined(GES_SHIPPING)
			glEnable(GL_DEBUG_OUTPUT);
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
			glDebugMessageCallback(opengl_message_callback, nullptr);
			glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_FALSE);
		#endif

		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glEnable(GL_DEPTH_TEST);
		// glDepthFunc(GL_LESS);
		// glDepthRangef(0.0f, 1.0f);
		// glClearDepth(1.0f);

		// glEnable(GL_CULL_FACE);
		// glFrontFace(GL_CCW);
		// glCullFace(GL_BACK);
	}

	Opengl_Renderer::~Opengl_Renderer() = default;

	void Opengl_Renderer::update()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
}

//
// platform implementation
//

static void opengl_message_callback(
	unsigned source,
	unsigned type,
	unsigned id,
	unsigned severity,
	int length,
	cstring message,
	void const * userParam)
{
	switch (severity)
	{
		case GL_DEBUG_SEVERITY_HIGH:         CUSTOM_ERROR("%s", message); return;
		case GL_DEBUG_SEVERITY_MEDIUM:       CUSTOM_WARN("%s", message); return;
		case GL_DEBUG_SEVERITY_LOW:          CUSTOM_INFO("%s", message); return;
		case GL_DEBUG_SEVERITY_NOTIFICATION: CUSTOM_TRACE("%s", message); return;
	}
	CUSTOM_ASSERT(false, "Unknown severity level!");
}
