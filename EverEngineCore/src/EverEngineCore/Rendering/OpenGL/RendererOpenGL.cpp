#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "RendererOpenGL.hpp"
#include "EverEngineCore/Log.hpp"

namespace EverEngine
{
    bool Renderer_OpenGL::init(GLFWwindow* pWindow)
    {
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            LOG_CRIT("ERROR::INIT::GLAD");
            return false;
        }
        return true;
    }

    void Renderer_OpenGL::draw(const VertexBuffer& vb)
    {

    }

    void Renderer_OpenGL::set_clear_color(const float r, const float g, const float b, const float a)
    {
        glClearColor(r, g, b, a);
    }

    void Renderer_OpenGL::clear()
    {
        glClear(GL_COLOR_BUFFER_BIT);
    }

    void Renderer_OpenGL::set_viewport(
        const unsigned int width, 
        const unsigned int height, 
        const unsigned int left_offset = 0,
        const unsigned int down_offset = 0)
    {
        glViewport(left_offset, down_offset, width, height);
    }
    
    void Renderer_OpenGL::enable_depth_test()
    {
        glEnable(GL_DEPTH_TEST);
    }

    void Renderer_OpenGL::disable_depth_test()
    {
        glDisable(GL_DEPTH_TEST);
    }

    const char* get_vendor_str() {};
    const char* get_renderer_str() {};
    const char* get_version_str() {};
}