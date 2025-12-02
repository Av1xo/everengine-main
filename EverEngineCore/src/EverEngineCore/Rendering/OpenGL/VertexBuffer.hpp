#ifndef VERTEX_BUFFER_HPP
#define VERTEX_BUFFER_HPP

#include "VertexLayout.hpp"

#include <glad/glad.h>
#include <cstddef>
#include <string>

namespace EverEngine
{

    // ========================================================================
    // Enums
    // ========================================================================
    
    enum class BufferUsage 
    {
        Static = GL_STATIC_DRAW,
        Dynamic = GL_DYNAMIC_DRAW,
        Stream = GL_STREAM_DRAW,
    };

    enum class DrawMode 
    {
        Triangles = GL_TRIANGLES,
        Lines = GL_LINES,
        Points = GL_POINTS,
        Patches = GL_PATCHES,
        TriangleStrip = GL_TRIANGLE_STRIP,
        LineStrip = GL_LINE_STRIP,
    };

    // ========================================================================
    // VertexBuffer
    // ========================================================================
    
    class VertexBuffer
    {
    public:
        VertexBuffer();
        VertexBuffer(const float* vertices, size_t vertexSize, size_t vertexCount,
                    const unsigned int* indices = nullptr, size_t indexCount = 0,
                    BufferUsage usage = BufferUsage::Static);
        
        ~VertexBuffer();

        VertexBuffer(const VertexBuffer&) = delete;
        VertexBuffer& operator=(const VertexBuffer&) = delete;
        
        VertexBuffer(VertexBuffer&& other) noexcept;
        VertexBuffer& operator=(VertexBuffer&& other) noexcept;

        void add_vertex_buffer(GLuint vbo, const VertexLayout& layout);

        GLuint get_vao() const { return m_vao; }
        size_t get_index_count() const { return m_indexCount; }
        size_t get_vertex_count() const { return m_vertexCount; }
        bool has_index_buffer() const { return m_ebo != 0; }
        
        void set_debug_name(const std::string& name) const;
        void set_data(const float* vertices, size_t vertexSize, size_t vertexCount,
                    const unsigned int* indices = nullptr, size_t indexCount = 0,
                    BufferUsage usage = BufferUsage::Static);
        void set_layout(const VertexLayout& layout);
        void set_vertex_attrib(GLuint index, GLint size, GLenum type, GLsizei stride, size_t offset);
        void set_indices(const unsigned int* indices, size_t count, BufferUsage usage);

        void update_data(size_t offset, const void* data, size_t size);

        void bind() const;
        void unbind() const;
        
        void draw(DrawMode mode = DrawMode::Triangles) const;
        void draw_instanced(GLsizei instanceCount, DrawMode mode = DrawMode::Triangles) const;

    private:
        GLuint m_vao;
        GLuint m_vbo;
        GLuint m_ebo;
        size_t m_indexCount;
        size_t m_vertexCount;
    };

} // namespace EverEngine

#endif // VERTEX_BUFFER_HPP