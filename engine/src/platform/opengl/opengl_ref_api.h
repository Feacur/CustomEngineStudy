void OpenGLRendererAPI::SetViewport(u32 x, u32 y, u32 width, u32 height)
{
	GES_PROFILE_FUNCTION();
	glViewport(x, y, width, height);
}

void OpenGLRendererAPI::DrawIndexed(Ref<VertexArray> const & vertexArray) const
{
	GES_PROFILE_FUNCTION();
	Ref<IndexBuffer> const & indexBuffer = vertexArray->GetIndexBuffer();
	glDrawElements(GL_TRIANGLES, indexBuffer->GetCount(), GL_UNSIGNED_INT, nullptr);
}

void OpenGLRendererAPI::DrawTriangle() const
{
	GES_PROFILE_FUNCTION();
	// send to a vertex shader indices [0, 1, 2]
	glDrawArrays(GL_TRIANGLES, 0, 3);
	// https://rauwendaal.net/2014/06/14/rendering-a-screen-covering-triangle-in-opengl/
	// https://twitter.com/nice_byte/status/1093355080235999232
}
