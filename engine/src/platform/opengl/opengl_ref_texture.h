OpenGLTexture2D::OpenGLTexture2D(u32 width, u32 height)
	: m_Width(width), m_Height(height)
{
	GES_PROFILE_FUNCTION();
	m_InternalFormat = GL_RGBA8;
	m_DataFormat = GL_RGBA;

	glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
	glTextureStorage2D(m_RendererID, 1, m_InternalFormat, m_Width, m_Height);

	glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

OpenGLTexture2D::OpenGLTexture2D(cstring source)
{
	GES_PROFILE_FUNCTION();
	s32 width, height, channels;
	stbi_set_flip_vertically_on_load(1);
	stbi_uc * data = stbi_load(source, &width, &height, &channels, 0);
	GES_CORE_ASSERT(data, "Failed to load image");

	m_Width = (u32)width;
	m_Height = (u32)height;

	if (channels == 1) {
		m_InternalFormat = GL_R8;
		m_DataFormat = GL_RED;
	}
	else if (channels == 2) {
		m_InternalFormat = GL_RG8;
		m_DataFormat = GL_RG;
	}
	else if (channels == 3) {
		m_InternalFormat = GL_RGB8;
		m_DataFormat = GL_RGB;
	}
	else if (channels == 4) {
		m_InternalFormat = GL_RGBA8;
		m_DataFormat = GL_RGBA;
	}
	else {
		m_InternalFormat = 0;
		m_DataFormat = 0;
		GES_CORE_ASSERT(false, "Failed to select image format");
	}

	glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
	glTextureStorage2D(m_RendererID, 1, m_InternalFormat, width, height);

	glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	
	glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTextureSubImage2D(m_RendererID, 0, 0, 0, width, height, m_DataFormat, GL_UNSIGNED_BYTE, data);

	stbi_image_free(data);
}

OpenGLTexture2D::~OpenGLTexture2D()
{
	GES_PROFILE_FUNCTION();
	glDeleteTextures(1, &m_RendererID);
}

void OpenGLTexture2D::SetData(void * data, u32 size)
{
	GES_PROFILE_FUNCTION();
	uint32_t bpp = m_DataFormat == GL_RGBA ? 4 : 3;
	GES_CORE_ASSERT(size == m_Width * m_Height * bpp, "Data must be entire texture!");
	glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, data);
}

void OpenGLTexture2D::Bind(u32 slot) const
{
	GES_PROFILE_FUNCTION();
	m_Slot = slot;
	glBindTextureUnit(slot, m_RendererID);
}

void OpenGLTexture2D::Unbind(u32 slot) const
{
	GES_PROFILE_FUNCTION();
	m_Slot = slot;
	glBindTextureUnit(slot, 0);
}
