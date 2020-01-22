typedef GLchar const * glstring;

static cstring const DefaultShader = R"(
// name default
// version 330 core
#if defined(VERTEX_SECTION)
layout(location = 0) in vec3 a_Position;
uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;
void main() { gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0); }
#endif // defined(VERTEX_SECTION)
#if defined(FRAGMENT_SECTION)
layout(location = 0) out vec4 color;
void main() { color = vec4(1.0, 0.0, 1.0, 1.0); }
#endif // defined(FRAGMENT_SECTION)
)";

static bool VerifyCompilation(GLuint shader, cstring name)
{
	GLint isCompiled = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled == GL_TRUE) { return true; }

	// @Note: linker will inform of the errors anyway
	GLint maxLength = 0;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

	std::vector<GLchar> infoLog(maxLength);
	glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);
	GES_CORE_ERROR("Shader compilation: '{0}'\n{1}", name, infoLog.data());

	return false;
}

static bool VerifyLinking(GLuint program, cstring name)
{
	GLint isLinked = 0;
	glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
	if (isLinked == GL_TRUE) { return true; }

	GLint maxLength = 0;
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

	std::vector<GLchar> infoLog(maxLength);
	glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);
	GES_CORE_ERROR("Shader linking: '{0}'\n{1}", name, infoLog.data());

	return false;
}

// u8 const code_count = C_ARRAY_LENGTH(code);
// GLint length[code_count];
// for (u8 i = 0; i < code_count; i++) {
// 	length[i] = (GLint)strlen(code[i]);
// }

// {
// 	GLint maxLength = 0;
// 	glGetShaderiv(shader, GL_SHADER_SOURCE_LENGTH, &maxLength);
// 	std::vector<GLchar> infoLog(maxLength);
// 	glGetShaderSource(shader, maxLength, &maxLength, &infoLog[0]);
// 	GES_CORE_TRACE("{0}", infoLog.data());
// }

struct ShaderProps
{
	GLenum   type;
	cstring version;
	cstring defines;
};

static GLuint CompileShader(cstring source, ShaderProps props)
{
	glstring code[] = { props.version, props.defines, source };
	GLuint shader = glCreateShader(props.type);
	glShaderSource(shader, C_ARRAY_LENGTH(code), code, 0);
	glCompileShader(shader);
	return shader;
}

static GLuint Compile(cstring source, cstring name)
{
	GES_PROFILE_FUNCTION();
	// @Todo: read this meta info from outside?
	static ShaderProps compilations_props[] = {
		{ GL_VERTEX_SHADER,   "#version 330 core\n", "#define VERTEX_SECTION\n" },
		{ GL_FRAGMENT_SHADER, "#version 330 core\n", "#define FRAGMENT_SECTION\n" },
		// { GL_GEOMETRY_SHADER, "#version 330 core\n", "#define GEOMETRY_SECTION\n" },
		// { GL_COMPUTE_SHADER,  "#version 430 core\n", "#define COMPUTE_SECTION\n" },
		// { GL_TESS_CONTROL_SHADER,    "#version 400 core\n", "#define TESSELATION_CONTROL_SECTION\n" },
		// { GL_TESS_EVALUATION_SHADER, "#version 400 core\n", "#define TESSELATION_EVALUATION_SECTION\n" },
	};
	u8 const compilations_props_count = C_ARRAY_LENGTH(compilations_props);

	// Compile shaders
	GLuint glShaderIDs[4] = {};
	for (u8 i = 0; i < compilations_props_count; i++)
	{
		glShaderIDs[i] = CompileShader(source, compilations_props[i]);
	}

	bool isCompiled = true;
	for (u8 i = 0; i < compilations_props_count; i++)
	{
		bool isOk = VerifyCompilation(glShaderIDs[i], name);
		isCompiled = isCompiled && isOk;
	}

	// Link the program
	GLuint program = glCreateProgram();
	for (u8 i = 0; i < compilations_props_count; i++) {
		glAttachShader(program, glShaderIDs[i]);
	}
	glLinkProgram(program);
	bool isLinked = VerifyLinking(program, name);

	// Free shader resources
	for (u8 i = 0; i < compilations_props_count; i++) {
		glDetachShader(program, glShaderIDs[i]);
	}
	for (u8 i = 0; i < compilations_props_count; i++) {
		glDeleteShader(glShaderIDs[i]);
	}

	if (!isCompiled || !isLinked) {
		glDeleteProgram(program);
		return 0;
	}

	return program;
}

OpenGLShader::OpenGLShader(cstring source, cstring name)
{
	GES_PROFILE_FUNCTION();
	m_RendererID = Compile(source, name);
	if (!m_RendererID) {
		m_RendererID = Compile(DefaultShader, "default");
	}
	m_Name = name;
}

OpenGLShader::~OpenGLShader()
{
	GES_PROFILE_FUNCTION();
	glDeleteProgram(m_RendererID);
}

void OpenGLShader::Bind() const
{
	GES_PROFILE_FUNCTION();
	glUseProgram(m_RendererID);
}

void OpenGLShader::Unbind() const
{
	GES_PROFILE_FUNCTION();
	glUseProgram(0);
}

void OpenGLShader::UploadUniformMat4(cstring name, glm::mat4 const & value)
{
	GES_PROFILE_FUNCTION();
	GLint location = glGetUniformLocation(m_RendererID, name);
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
}

void OpenGLShader::UploadUniformInt(cstring name, s32 value)
{
	GES_PROFILE_FUNCTION();
	GLint location = glGetUniformLocation(m_RendererID, name);
	glUniform1i(location, value);
}

void OpenGLShader::UploadUniformFloat1(cstring name, float value)
{
	GES_PROFILE_FUNCTION();
	GLint location = glGetUniformLocation(m_RendererID, name);
	glUniform1f(location, value);
}

void OpenGLShader::UploadUniformFloat2(cstring name, glm::vec2 const & value)
{
	GES_PROFILE_FUNCTION();
	GLint location = glGetUniformLocation(m_RendererID, name);
	glUniform2f(location, value.x, value.y);
}

void OpenGLShader::UploadUniformFloat3(cstring name, glm::vec3 const & value)
{
	GES_PROFILE_FUNCTION();
	GLint location = glGetUniformLocation(m_RendererID, name);
	glUniform3f(location, value.x, value.y, value.z);
}

void OpenGLShader::UploadUniformFloat4(cstring name, glm::vec4 const & value)
{
	GES_PROFILE_FUNCTION();
	GLint location = glGetUniformLocation(m_RendererID, name);
	glUniform4f(location, value.x, value.y, value.z, value.w);
}
