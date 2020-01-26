// name: flat_color
// version: 330 core

// section: VERTEX_SECTION
#if defined(VERTEX_SECTION)
layout(location = 0) in vec3 a_Position;

uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;

void main()
{
	gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
}
#endif // defined(VERTEX_SECTION)

// section: FRAGMENT_SECTION
#if defined(FRAGMENT_SECTION)
uniform vec4 u_Color;

layout(location = 0) out vec4 color;

void main()
{
	color = u_Color;
}
#endif // defined(FRAGMENT_SECTION)
