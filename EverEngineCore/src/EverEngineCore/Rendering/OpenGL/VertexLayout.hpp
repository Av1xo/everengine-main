#ifndef VERTEX_LAYOUT_HPP
#define VERTEX_LAYOUT_HPP

#include <glad/glad.h>
#include <vector>
#include <string>

namespace EverEngine
{
    struct VertexAttribute 
    {
        GLuint index;
        GLint size;
        GLenum type;
        bool normalized; 
    };

    class VertexLayout
    {
    public:
        std::vector<VertexAttribute> attributes;
        GLsizei stride = 0;

        void push(GLint size, GLenum type, bool normalized = GL_FALSE) 
        {
            attributes.push_back({static_cast<GLuint>(attributes.size()), size, type, normalized});
            
            GLsizei typeSize = 0;
            switch (type)
            {
                case GL_FLOAT: typeSize = sizeof(GLfloat); break;
                case GL_INT: typeSize = sizeof(GLint); break;
                case GL_UNSIGNED_INT: typeSize = sizeof(GLuint); break;
                case GL_BYTE: typeSize = sizeof(GLbyte); break;
                case GL_UNSIGNED_BYTE: typeSize = sizeof(GLubyte); break;
                case GL_SHORT: typeSize = sizeof(GLshort); break;
                case GL_UNSIGNED_SHORT: typeSize = sizeof(GLushort); break;
                default: typeSize = sizeof(GLfloat); break;
            }

            stride += size * typeSize;
        }
    };
}

#endif