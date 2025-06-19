#include "enginepch.h"
#include "Platform/OpenGL/Buffer/OpenGLIndexBuffer.h"

#include <GL/glew.h>

namespace pixc {

/**
 * @brief Generate an index buffer and link it to the input indices.
 *
 * @param indices Index information for the vertices.
 * @param count Number of indices.
 */
OpenGLIndexBuffer::OpenGLIndexBuffer(const uint32_t *indices,
                                     const uint32_t count)
: IndexBuffer(count)
{
    glGenBuffers(1, &m_ID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)(count * sizeof(uint32_t)),
                 indices, GL_STATIC_DRAW);
}

/**
 * @brief Delete the index buffer.
 */
OpenGLIndexBuffer::~OpenGLIndexBuffer()
{
    glDeleteBuffers(1, &m_ID);
}

/**
 * @brief Bind the index buffer.
 */
void OpenGLIndexBuffer::Bind() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ID);
}

/**
 * @brief Unbind the index buffer.
 */
void OpenGLIndexBuffer::Unbind() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

} // namespace pixc
