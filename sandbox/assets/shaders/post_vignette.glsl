// name: post_vignette
// version: 330 core

// section: VERTEX_SECTION
#if defined(VERTEX_SECTION)
// https://twitter.com/_kzr/status/829621272757755905
// http://www.loopit.dk/banding_in_games.pdf
// http://www.anisopteragames.com/how-to-fix-color-banding-with-dithering/
// https://www.shadertoy.com/view/MlV3R1
// https://computergraphics.stackexchange.com/questions/3964/opengl-specular-shading-gradient-banding-issues
// https://gist.github.com/pixelmager/5d25fa32987273b9608a2d2c6cc74bfa
// https://github.com/playdeadgames/publications/blob/master/INSIDE/rendering_inside_gdc2016.pdf
// http://momentsingraphics.de/BlueNoise.html

#define VERTEX_MODE 1

const float NearClipValue = -1;
const float FarClipValue  =  1;

#if VERTEX_MODE == 1
// no input
#else
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;
#endif

uniform vec2 u_ScreenSize;
uniform sampler2D u_Texture;

out vec2 v_TexCoord;
out vec2 v_ScreenPos;

#if VERTEX_MODE == 1
void main()
{
	// gl_VertexID == 0 -> (0, 0)
	// gl_VertexID == 1 -> (2, 0)
	// gl_VertexID == 2 -> (0, 2)
	v_TexCoord = ivec2(gl_VertexID << 1, gl_VertexID) & 2;
	// map the vertices to cover whole NDC
	v_ScreenPos = v_TexCoord * 2 - 1;
	// map texture pixel-to-pixel with the framebuffer, assuming wrap-repeat mode
	v_TexCoord *= textureSize(u_Texture, 0);
	// compensate aspect ratio
	v_TexCoord.x *= u_ScreenSize.x / u_ScreenSize.y;
	// display in front of everything
	gl_Position = vec4(v_ScreenPos, NearClipValue, 1);
	// https://rauwendaal.net/2014/06/14/rendering-a-screen-covering-triangle-in-opengl/
	// https://twitter.com/nice_byte/status/1093355080235999232
}
#else
void main()
{
	// map the vertices to cover whole NDC, assuming a_Position is x[-0.5..0.5]:y[-0.5..0.5]
	v_ScreenPos = a_Position.xy * 2;
	// map texture pixel-to-pixel with the framebuffer, assuming wrap-repeat mode
	v_TexCoord = a_TexCoord * textureSize(u_Texture, 0);
	// compensate aspect ratio, assuming a_TexCoord is x[0..1]:y[0..1]
	v_TexCoord.x *= u_ScreenSize.x / u_ScreenSize.y;
	// display in front of everything
	gl_Position = vec4(v_ScreenPos, -1, 1);
	// https://www.khronos.org/opengl/wiki/Vertex_Processing
	// https://www.khronos.org/opengl/wiki/Vertex_Post-Processing
	// https://www.khronos.org/opengl/wiki/Built-in_Variable_(GLSL)
	// https://www.khronos.org/opengl/wiki/GLAPI/glDepthRange
	// https://www.scratchapixel.com/lessons/3d-basic-rendering/perspective-and-orthographic-projection-matrix/projection-matrix-GPU-rendering-pipeline-clipping?url=3d-basic-rendering/perspective-and-orthographic-projection-matrix/projection-matrix-GPU-rendering-pipeline-clipping
	// https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glClipControl.xhtml
}
#endif
#endif // defined(VERTEX_SECTION)

// section: FRAGMENT_SECTION
#if defined(FRAGMENT_SECTION)
#define FRAG_OUT_MODE 1

in vec2 v_TexCoord;
in vec2 v_ScreenPos;

uniform vec4 u_Color;
uniform sampler2D u_Texture;

layout(location = 0) out vec4 color;

void main()
{
	float vignette_value = distance(v_ScreenPos, vec2(0, 0));
	vignette_value = clamp(vignette_value - 0.4, 0, 1);
	vignette_value *= vignette_value;

	float dither_value = texture(u_Texture, v_TexCoord).r;

	#if FRAG_OUT_MODE == 1
		// jitter color with dither; blend by input alpha and vignette
		// @Note: might restrict jittering RGB or Alpha only, too
		color = vec4(u_Color.rgb, u_Color.a * vignette_value) + (dither_value - 0.5) / 8;
	#else
		// default; blend by input alpha and vignette
		color = vec4(u_Color.rgb, u_Color.a * vignette_value);
	#endif
}
#endif // defined(FRAGMENT_SECTION)
