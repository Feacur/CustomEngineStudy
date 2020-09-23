// section: VERTEX_SECTION
#if defined(VERTEX_SECTION)
layout(location = 0) in vec3 a_Position;

uniform mat4 u_View_Projection;
uniform mat4 u_Transform;

void main()
{
	// @Note: expects column-major matrices
	//        u_View_Projection == camera_projection * camera_inverse_transform
	gl_Position = vec4(a_Position, 1.0) * u_Transform * u_View_Projection;
}
#endif // defined(VERTEX_SECTION)

// section: FRAGMENT_SECTION
#if defined(FRAGMENT_SECTION)
uniform vec4 u_Color = vec4(1, 1, 1, 1);

layout(location = 0) out vec4 color;

void main()
{
	color = u_Color;
}
#endif // defined(FRAGMENT_SECTION)
