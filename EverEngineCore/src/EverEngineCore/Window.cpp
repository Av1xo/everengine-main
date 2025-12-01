#include "EverEngineCore/Window.hpp"
#include "EverEngineCore/Log.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>



#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_opengl3.h>

namespace EverEngine
{
    static bool s_GLFW_initialised = false;

    Window::Window(const std::string& title, const unsigned int width,
        const unsigned int height)
        : m_data({std::move(title), width, height})
    {
        int resultCode = init();

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui_ImplOpenGL3_Init();
    }

    Window::~Window()
    {
        shutdown();
    }

    void Window::set_event_callback(const EventCallbackFn& callback){
        m_data.eventCallbackFn = callback;
    }

    int Window::init()
    {
        LOG_INFO("CREATE::WINDOW: '{0}' {1}x{2}", m_data.title, m_data.width, m_data.height);

        if (!s_GLFW_initialised)
        {
            if(!glfwInit())
            {
                LOG_CRIT("ERROR::INIT::GLFW");
                return -1;
            }
            s_GLFW_initialised = true;
        }

        m_pWindow = glfwCreateWindow(m_data.width, m_data.height, m_data.title.c_str(), nullptr, nullptr);
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

        glfwSetWindowUserPointer(m_pWindow, &m_data);

        glfwSetWindowSizeCallback(m_pWindow,
            [](GLFWwindow* pWindow, int width, int height)
            {
                WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(pWindow));
                data.width = width;
                data.height = height;

                data.eventCallbackFn(std::make_unique<EventWindowResize>(width, height));
            }
        );

        glfwSetCursorPosCallback(m_pWindow,
            [](GLFWwindow* pWindow, double x, double y)
            {
                WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(pWindow));

                data.eventCallbackFn(std::make_unique<EventMouseMoved>(x, y));
            }
        );

        glfwSetWindowCloseCallback(m_pWindow,
            [](GLFWwindow* pWindow){
                WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(pWindow));

                data.eventCallbackFn(std::make_unique<EventWindowClose>());
            }
        );
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

        ImGuiIO& io = ImGui::GetIO();
        io.DisplaySize.x = static_cast<float>(get_width());
        io.DisplaySize.y = static_cast<float>(get_height());

        ImGui_ImplOpenGL3_NewFrame();
        ImGui::NewFrame();

        ImGui::ShowDemoWindow();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(m_pWindow);
        glfwPollEvents();
    }
}