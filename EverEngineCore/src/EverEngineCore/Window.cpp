#include "EverEngineCore/Window.hpp"
#include "EverEngineCore/Log.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>


namespace EverEngine
{
    static bool s_GLFW_initialised = false;

    Window::Window(const char* title, const unsigned int width,
        const unsigned int height)
        : m_title(title)
        , m_width(width)
        , m_height(height)
    {
        int resultCode = init();
    }

    Window::~Window()
    {
        shutdown();
    }

    int Window::init()
    {
        LOG_INFO("CREATE::WINDOW: '{0}' {1}x{2}", m_title, m_width, m_height);

        if (!s_GLFW_initialised)
        {
            if(!glfwInit())
            {
                LOG_CRIT("ERROR::INIT::GLFW");
                return -1;
            }
            s_GLFW_initialised = true;
        }

        m_pWindow = glfwCreateWindow(m_width, m_height, m_title, nullptr, nullptr);
        if(!m_pWindow)
        {
            LOG_CRIT("ERROR::CREATE::WINDOW");
            glfwTerminate();
            return -2;
        }

        glfwMakeContextCurrent(m_pWindow);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            LOG_CRIT("ERROR::INIT::GLAD");
            return -3;
        }

        return 0;
    }

    void Window::shutdown()
    {
        glfwDestroyWindow(m_pWindow);
        glfwTerminate();
    }

    void Window::on_update()
    {
        glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        glfwSwapBuffers(m_pWindow);
        glfwPollEvents;
    }
}