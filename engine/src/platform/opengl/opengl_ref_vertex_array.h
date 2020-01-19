static GLenum ShaderDataTypeOpenGLBaseType(ShaderDataType type)
{
	GES_PROFILE_FUNCTION();
	switch(type)
	{
		case ShaderDataType::Float1: return GL_FLOAT;
		case ShaderDataType::Float2: return GL_FLOAT;
		case ShaderDataType::Float3: return GL_FLOAT;
		case ShaderDataType::Float4: return GL_FLOAT;
		case ShaderDataType::Mat3:   return GL_FLOAT;
		case ShaderDataType::Mat4:   return GL_FLOAT;
		case ShaderDataType::Int1:   return GL_INT;
		case ShaderDataType::Int2:   return GL_INT;
		case ShaderDataType::Int3:   return GL_INT;
		case ShaderDataType::Int4:   return GL_INT;
		case ShaderDataType::Bool:   return GL_BOOL;
	}
	GES_CORE_ASSERT(false, "unsupported ShaderDataType '{0}'", (s32)type);
	return 0;
}

OpenGLVertexArray::OpenGLVertexArray()
{
	GES_PROFILE_FUNCTION();
	glGenVertexArrays(1, &m_RendererID);
	glBindVertexArray(m_RendererID);
}

OpenGLVertexArray::~OpenGLVertexArray()
{
	GES_PROFILE_FUNCTION();
	glDeleteVertexArrays(1, &m_RendererID);
}

void OpenGLVertexArray::Bind() const
{
	GES_PROFILE_FUNCTION();
	glBindVertexArray(m_RendererID);
}

void OpenGLVertexArray::Unbind() const
{
	GES_PROFILE_FUNCTION();
	glBindVertexArray(0);
}

void OpenGLVertexArray::AddVertexBuffer(Ref<VertexBuffer> const & vertexBuffer)
{
	GES_PROFILE_FUNCTION();
	BufferLayout const & bufferLayout = vertexBuffer->GetLayout();
	GES_CORE_ASSERT(bufferLayout.GetElements().size(), "Vertex Buffer has no layout");

	glBindVertexArray(m_RendererID);
	vertexBuffer->Bind();
	m_VertexBuffers.push_back(vertexBuffer);
	
	u32 stride = 0;
	for (auto & element : bufferLayout)
	{
		stride += element.GetSize();
	}

	uintptr_t offset = 0;
	u32 vertexAttribIndex = 0;
	for (auto const & element : bufferLayout)
	{
		glEnableVertexAttribArray(vertexAttribIndex);
		glVertexAttribPointer(
			vertexAttribIndex,
			element.GetComponentCount(),
			ShaderDataTypeOpenGLBaseType(element.Type),
			element.Normalized ? GL_TRUE : GL_FALSE,
			stride,
			(void const *)offset
		);
		offset += element.GetSize();
		vertexAttribIndex++;
	}
}

void OpenGLVertexArray::SetIndexBuffer(Ref<IndexBuffer> const & indexBuffer)
{
	GES_PROFILE_FUNCTION();
	glBindVertexArray(m_RendererID);
	indexBuffer->Bind();
	m_IndexBuffer = indexBuffer;
}
