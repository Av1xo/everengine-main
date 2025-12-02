#include "VertexBuffer.hpp"
#include <iostream>

namespace EverEngine
{
    VertexBuffer::VertexBuffer()
    {
        glGenVertexArrays(1, &m_vao);
    }

    VertexBuffer::VertexBuffer(const float* vertices, size_t vertexSize, size_t vertexCount, 
        const unsigned int* indices, size_t indexCount,
        BufferUsage usage) : VertexBuffer()
        {
            set_data(vertices, vertexSize, vertexCount, indices, indexCount, usage);
        }

    VertexBuffer::~VertexBuffer()
    {
        if (m_vao != 0) glDeleteVertexArrays(1, &m_vao);
        if (m_vbo != 0) glDeleteBuffers(1, &m_vbo);
        if (m_ebo != 0) glDeleteBuffers(1, &m_ebo);
    }

    VertexBuffer::VertexBuffer(VertexBuffer&& other) noexcept
        : m_vao(other.m_vao)
        , m_vbo(other.m_vbo)
        , m_ebo(other.m_ebo)
        , m_indexCount(other.m_indexCount)
        , m_vertexCount(other.m_vertexCount)
    {
        other.m_vao = 0;
        other.m_vbo = 0;
        other.m_ebo = 0;
        other.m_indexCount = 0;
        other.m_vertexCount = 0;
    }

    VertexBuffer& VertexBuffer::operator=(VertexBuffer&& other) noexcept
    {
        if (this != &other)
        {
            if (m_vao != 0) glDeleteVertexArrays(1, &m_vao);
            if (m_vbo != 0) glDeleteBuffers(1, &m_vbo);
            if (m_ebo != 0) glDeleteBuffers(1, &m_ebo);

            m_vao = other.m_vao;
            m_vbo = other.m_vbo;
            m_ebo = other.m_ebo;
            m_indexCount = other.m_indexCount;
            m_vertexCount = other.m_vertexCount; 

            other.m_vao = 0;
            other.m_vbo = 0;
            other.m_ebo = 0;
            other.m_indexCount = 0;
            other.m_vertexCount = 0; 
        }
        return *this;
    }

    void VertexBuffer::set_data(const float* vertices, size_t vertexSize, size_t vertexCount,
        const unsigned int* indices, size_t indexCount, BufferUsage usage)
    {
        bind();

        if (m_vbo == 0){
            glGenBuffers(1, &m_vbo);
        }
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER, vertexSize, vertices, static_cast<GLenum>(usage));

        m_vertexCount = vertexCount;

        if (indices && indexCount > 0)
        {
            if (m_ebo == 0) {
                glGenBuffers(1, &m_ebo);
            }
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(unsigned int),
                indices, static_cast<GLenum>(usage));
            m_indexCount = indexCount;
        }

        unbind();
    }

    void VertexBuffer::set_layout(const VertexLayout& layout)
    {
        bind();
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

        size_t offset = 0;
        for (const auto& attrib : layout.attributes)
        {
            glEnableVertexAttribArray(attrib.index);
            glVertexAttribPointer(
                attrib.index,
                attrib.size,
                attrib.type,
                attrib.normalized,
                layout.stride,
                (void*)offset
            );

            GLsizei typeSize = 0;
            switch(attrib.type) {
                case GL_FLOAT: typeSize = sizeof(GLfloat); break;
                case GL_INT: typeSize = sizeof(GLint); break;
                case GL_UNSIGNED_INT: typeSize = sizeof(GLuint); break;
                case GL_BYTE: typeSize = sizeof(GLbyte); break;
                case GL_UNSIGNED_BYTE: typeSize = sizeof(GLubyte); break;
                default: typeSize = sizeof(GLfloat); break;
            }
            offset += attrib.size * typeSize;
        }

        unbind();
    }

    void VertexBuffer::add_vertex_buffer(GLuint vbo, const VertexLayout& layout)
    {
        bind();
        glBindBuffer(GL_ARRAY_BUFFER, vbo);

        size_t offset = 0;
        for (const auto& attrib : layout.attributes)
        {
            glEnableVertexAttribArray(attrib.index);
            glVertexAttribPointer(
                attrib.index, attrib.size, attrib.type,
                attrib.normalized, layout.stride, (void*)offset
            );
            
            GLsizei typeSize = 0;
            switch(attrib.type) {
                case GL_FLOAT: typeSize = sizeof(GLfloat); break;
                case GL_INT: typeSize = sizeof(GLint); break;
                case GL_UNSIGNED_INT: typeSize = sizeof(GLuint); break;
                case GL_BYTE: typeSize = sizeof(GLbyte); break;
                case GL_UNSIGNED_BYTE: typeSize = sizeof(GLubyte); break;
                default: typeSize = sizeof(GLfloat); break;
            }
            offset += attrib.size * typeSize;
        }
        
        unbind();
    }

    void VertexBuffer::set_vertex_attrib(GLuint index, GLint size, GLenum type, 
        GLsizei stride, size_t offset)
    {
        bind();
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glEnableVertexAttribArray(index);
        glVertexAttribPointer(index, size, type, GL_FALSE, stride, (void*)offset);
        unbind();
    }

    void VertexBuffer::set_indices(const unsigned int* indices, size_t count, BufferUsage usage)
    {
        bind();
        
        if (m_ebo == 0) {
            glGenBuffers(1, &m_ebo);
        }
        
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), 
                    indices, static_cast<GLenum>(usage));
        m_indexCount = count;
        
        unbind();
    }

    void VertexBuffer::update_data(size_t offset, const void* data, size_t size)
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
    }

    void VertexBuffer::bind() const
    {
        glBindVertexArray(m_vao);
    }

    void VertexBuffer::unbind() const
    {
        glBindVertexArray(0);
    }

    void VertexBuffer::draw(DrawMode mode) const
    {
        bind();

        if (has_index_buffer())
        {
            glDrawElements(static_cast<GLenum>(mode), m_indexCount, GL_UNSIGNED_INT, 0);
        }
        else 
        {
            glDrawArrays(static_cast<GLenum>(mode), 0, m_vertexCount);
        }

        unbind();
    }

    void VertexBuffer::draw_instanced(GLsizei instanceCount, DrawMode mode) const
    {
        bind();
        
        if (has_index_buffer())
        {
            glDrawElementsInstanced(static_cast<GLenum>(mode), m_indexCount, 
                                GL_UNSIGNED_INT, 0, instanceCount);
        }
        else
        {
            glDrawArraysInstanced(static_cast<GLenum>(mode), 0, m_vertexCount, instanceCount);
        }
        
        unbind();
    }

    void VertexBuffer::set_debug_name(const std::string& name) const
    {
    #ifdef GL_KHR_debug
        if (glObjectLabel)
        {
            glObjectLabel(GL_VERTEX_ARRAY, m_vao, -1, name.c_str());
        }
    #endif
    }
}
