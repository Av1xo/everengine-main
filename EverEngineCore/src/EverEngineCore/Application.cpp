#include "EverEngineCore/Application.hpp"
#include "EverEngineCore/Log.hpp"
#include "EverEngineCore/Window.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace EverEngine
{
    Application::Application()
    {
        LOG_INFO("START::APPLICATION");
    }

    Application::~Application()
    {
        LOG_INFO("CLOSE::APPLICATION");
    }

    int Application::start(unsigned int window_width, unsigned int window_height, const char* title)
    {
        m_pWindow = std::make_unique<Window>(title, window_width, window_height);
        /* Loop until the user closes the window */
        while (true)
        {
            m_pWindow->on_update();
            on_update();
        }

        glfwTerminate();
        return 0;
    }
}