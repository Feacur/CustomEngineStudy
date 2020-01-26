// name: post_vignette
// version: 330 core

// section: VERTEX_SECTION
#if defined(VERTEX_SECTION)
// https://en.wikipedia.org/wiki/Stippling
// https://en.wikipedia.org/wiki/Ordered_dithering
// http://www.tannerhelland.com/4660/dithering-eleven-algorithms-source-code/

// https://twitter.com/_kzr/status/829621272757755905
// http://www.loopit.dk/banding_in_games.pdf
// http://www.anisopteragames.com/how-to-fix-color-banding-with-dithering/
// https://www.shadertoy.com/view/MlV3R1
// https://computergraphics.stackexchange.com/questions/3964/opengl-specular-shading-gradient-banding-issues
// https://gist.github.com/pixelmager/5d25fa32987273b9608a2d2c6cc74bfa
// https://github.com/playdeadgames/publications/blob/master/INSIDE/rendering_inside_gdc2016.pdf
// http://momentsingraphics.de/BlueNoise.html

// https://youtu.be/IviNO7iICTM
// https://youtu.be/--GB9qyZJqg
// https://digitalrune.github.io/DigitalRune-Documentation/html/fa431d48-b457-4c70-a590-d44b0840ab1e.htm
// https://ocias.com/blog/unity-stipple-transparency-shader/

#define VERTEX_MODE 1

const float NearClipValue = -1;
const float FarClipValue  =  1;

#if VERTEX_MODE == 1
// no input
#else
layout(location = 0) in vec3 a_Position;
#endif

uniform vec2 u_ScreenSize;

out vec2 v_ScreenPos;

#if VERTEX_MODE == 1
void main()
{
	// gl_VertexID == 0 -> (0, 0)
	// gl_VertexID == 1 -> (2, 0)
	// gl_VertexID == 2 -> (0, 2)
	vec2 TexCoord = ivec2(gl_VertexID << 1, gl_VertexID) & 2;
	// map the vertices to cover whole NDC
	v_ScreenPos = TexCoord * 2 - 1;
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
#define DITHER_MODE 8
#define FRAG_OUT_MODE 2

in vec2 v_ScreenPos;

uniform vec4 u_Color;

layout(location = 0) out vec4 color;

#if DITHER_MODE == 4
	// @Note: might be a texture, array, matrix
	uniform float u_Threshold[DITHER_MODE * DITHER_MODE] = {
		ThresholdLine(float[4]( 0/16.,  8/16.,  2/16., 10/16.)),
		ThresholdLine(float[4](12/16.,  4/16., 14/16.,  6/16.)),
		ThresholdLine(float[4]( 3/16., 11/16.,  1/16.,  9/16.)),
		ThresholdLine(float[4](15/16.,  7/16., 13/16.,  5/16.))
	);
#elif DITHER_MODE == 8
	// @Note: might be a texture, array
	uniform float u_Threshold[DITHER_MODE * DITHER_MODE] = {
		 0/64., 32/64.,  8/64., 40/64.,  2/64., 34/64., 10/64., 42/64.,
		48/64., 16/64., 56/64., 24/64., 50/64., 18/64., 58/64., 26/64.,
		12/64., 44/64.,  4/64., 36/64., 14/64., 46/64.,  6/64., 38/64.,
		60/64., 28/64., 52/64., 20/64., 62/64., 30/64., 54/64., 22/64.,
		 3/64., 35/64., 11/64., 43/64.,  1/64., 33/64.,  9/64., 41/64.,
		51/64., 19/64., 59/64., 27/64., 49/64., 17/64., 57/64., 25/64.,
		15/64., 47/64.,  7/64., 39/64., 13/64., 45/64.,  5/64., 37/64.,
		63/64., 31/64., 55/64., 23/64., 61/64., 29/64., 53/64., 21/64.
	};
#elif DITHER_MODE == -1
	uint hash_jenkins(uint x) {
		x += x << 10u;
		x ^= x >>  6u;
		x += x <<  3u;
		x ^= x >> 11u;
		x += x << 15u;
		return x;
	}
	uint hash_jenkins(uvec2 v) { return hash_jenkins(v.x ^ hash_jenkins(v.y)); }

	float uint_bits_to_01(uint x) {
		// @Note: might well mask fractional part with [0x007fffffU] instead of shifting
		return uintBitsToFloat((x >> 9) | 0x3f800000U) - 1.0; // return [1 .. 2) * (2^0) - 1
	}
	float uint_bits_to_radius01(uint x) {
		// @Note: might well mask fractional part with [0x007fffffU] instead of shifting
		return uintBitsToFloat(x = (x >> 9) | 0x40000000U) - 3.0; // return [1 .. 2) * (2^1) - 3
	}

	float random_01(vec2 v) { return uint_bits_to_01(hash_jenkins(floatBitsToUint(v))); }
	float random_radius01(vec2 v) { return uint_bits_to_radius01(hash_jenkins(floatBitsToUint(v))); }
#endif

void main()
{
	float vignette_value = distance(v_ScreenPos, vec2(0));
	vignette_value = clamp(vignette_value - 0.4, 0, 1);
	vignette_value *= vignette_value;

	#if DITHER_MODE == 4 || DITHER_MODE == 8
		// sample the threshold matrix
		ivec2 dither_coord = ivec2(gl_FragCoord.xy) % DITHER_MODE;
		float dither_value = u_Threshold[dither_coord.y * DITHER_MODE + dither_coord.x];
	#elif DITHER_MODE == -1
		// sample random value
		float dither_value = random_01(gl_FragCoord.xy);
	#else
		// default to zero
		float dither_value = 0;
	#endif

	#if FRAG_OUT_MODE == 1
		// mask with vignette and input alpha; do not blend
		if (u_Color.a * vignette_value <= dither_value) discard;
		color = vec4(u_Color.rgb, 1);
	#elif FRAG_OUT_MODE == 2
		// mask with vignette only; blend by input alpha
		if (vignette_value <= dither_value) discard;
		color = u_Color;
	#elif FRAG_OUT_MODE == 3
		// control input alpha with vignette and dither; blend by the result
		// @Note: will never reach full opacity
		color = vec4(u_Color.rgb, u_Color.a * vignette_value * dither_value);
	#elif FRAG_OUT_MODE == 4
		// control input alpha with vignette mixed by dither; blend by the result
		// @Note: dither pattern is omnipresent
		color = vec4(u_Color.rgb, u_Color.a * mix(1, vignette_value, dither_value));
	#elif FRAG_OUT_MODE == 5
		// control input alpha with dither mixed by vignette; blend by the result
		// @Note: dither pattern is omnipresent
		color = vec4(u_Color.rgb, u_Color.a * mix(dither_value, 1, vignette_value));
	#elif FRAG_OUT_MODE == 6
		// jitter color with dither; blend by input alpha and vignette
		// @Note: might restrict jittering RGB or Alpha only, too
		color = vec4(u_Color.rgb, u_Color.a * vignette_value) + (dither_value - 0.5) / 8;
	#else
		// default; blend by input alpha and vignette
		color = vec4(u_Color.rgb, u_Color.a * vignette_value);
	#endif
}
#endif // defined(FRAGMENT_SECTION)
