// section: VERTEX_SECTION
#if defined(VERTEX_SECTION)
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;
layout(location = 2) in vec3 a_Normal;

uniform mat4 u_View_Projection;
uniform mat4 u_Transform;

out vec2 v_TexCoord;
out vec3 v_Normal;

void main()
{
	v_TexCoord = a_TexCoord;
	v_Normal = (u_Transform * vec4(a_Normal, 0.0)).xyz;
	gl_Position = u_View_Projection * u_Transform * vec4(a_Position, 1.0);
}
#endif // defined(VERTEX_SECTION)

// section: FRAGMENT_SECTION
#if defined(FRAGMENT_SECTION)
in vec2 v_TexCoord;
in vec3 v_Normal;

uniform vec4 u_Color = vec4(1, 1, 1, 1);
uniform sampler2D u_Texture;

layout(location = 0) out vec4 color;

void main()
{
	color = texture(u_Texture, v_TexCoord) * u_Color;
	// color.rgb = mix(color.rgb, (v_Normal + 1) / 2, 1);
}
#endif // defined(FRAGMENT_SECTION)
