// section: VERTEX_SECTION
#if defined(VERTEX_SECTION)
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;

uniform mat4 u_View_Projection;
uniform mat4 u_Transform;

out vec4 v_Color;

void main()
{
	// @Note: expects column-major matrices
	//        u_View_Projection == camera_projection * camera_inverse_transform
	v_Color = a_Color;
	gl_Position = vec4(a_Position, 1.0) * u_Transform * u_View_Projection;
}
#endif // defined(VERTEX_SECTION)

// section: FRAGMENT_SECTION
#if defined(FRAGMENT_SECTION)
in vec4 v_Color;

layout(location = 0) out vec4 color;

void main()
{
	color = v_Color;
}
#endif // defined(FRAGMENT_SECTION)
