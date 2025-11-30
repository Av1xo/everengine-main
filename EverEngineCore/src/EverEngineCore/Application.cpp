#include "EverEngineCore/Application.hpp"
#include "EverEngineCore/Log.hpp"

#include <GLFW/glfw3.h>

namespace EverEngine
{
    Application::Application()
    {
        LOG_INFO("Welcome to spdlog!");
        LOG_ERROR("Some error message with arg: {}", 1);
        
        LOG_WARN("Easy padding in numbers like {:08d}", 12);
        LOG_CRIT("Support for int: {0:d};  hex: {0:x};  oct: {0:o}; bin: {0:b}", 42);
    }

    Application::~Application()
    {

    }

    int Application::start(unsigned int window_width, unsigned int window_height, const char* title)
    {
        GLFWwindow* window;

        /* Initialize the library */
        if (!glfwInit())
            return -1;

        /* Create a windowed mode window and its OpenGL context */
        window = glfwCreateWindow(window_width, window_height, title, NULL, NULL);
        if (!window)
        {
            glfwTerminate();
            return -1;
        }

        /* Make the window's context current */
        glfwMakeContextCurrent(window);

        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window))
        {
            /* Render here */
            // glClear(GL_COLOR_BUFFER_BIT);

            /* Swap front and back buffers */
            glfwSwapBuffers(window);

            /* Poll for and process events */
            glfwPollEvents();

            on_update();
        }

        glfwTerminate();
        return 0;
    }
}