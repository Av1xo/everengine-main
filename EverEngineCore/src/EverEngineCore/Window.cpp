#include "Window.hpp"
#include "EverEngineCore/Log.hpp"
#include "Rendering/OpenGL/Shader.hpp"
#include "Rendering/OpenGL/VertexBuffer.hpp"


#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <imgui/backends/imgui_impl_glfw.h>

namespace EverEngine
{

    static std::unique_ptr<Shader> s_shader;
    static std::unique_ptr<VertexBuffer> s_vbo;


    float s_vertices[] = {
        -0.5f, -0.5f, 0.0f,     1.0f, 0.0f, 0.0f,
            0.5f, -0.5f, 0.0f,     0.5f, 1.0f, 0.0f,
            0.0f,  0.5f, 0.0f,     0.0f, 0.0f, 1.0f
    };


#define ENGINE_DEBUG
    static bool s_GLFW_initialised = false;

    Window::Window(const std::string& title, const unsigned int width,
        const unsigned int height)
        : m_data({std::move(title), width, height})
    {
        int resultCode = init();
#ifdef ENGINE_DEBUG
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui_ImplOpenGL3_Init();
        ImGui_ImplGlfw_InitForOpenGL(m_pWindow, true);
#endif
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

        glfwSetFramebufferSizeCallback(m_pWindow, 
            [](GLFWwindow* pWindow, int width, int height)
            {
                glViewport(0, 0, width, height);
            }
        );

        s_shader = std::make_unique<Shader>(
            std::unordered_map<unsigned int, const char*>{
                {GLShaderType::Vertex, "shaders/vertex.vert"},
                {GLShaderType::Fragment, "shaders/fragment.frag"}
            }
        );

        s_vbo = std::make_unique<VertexBuffer>(s_vertices, sizeof(s_vertices), 3, nullptr, 0, BufferUsage::Dynamic);

        VertexLayout layout;
        layout.push(3, GL_FLOAT); // pos
        layout.push(3, GL_FLOAT); // color
        s_vbo->set_layout(layout);
        return 0;
    }

    void Window::shutdown()
    {
        glfwDestroyWindow(m_pWindow);
        glfwTerminate();

    }

    void Window::on_update()
    {
        glClearColor(m_backgroundColor[0], m_backgroundColor[1], m_backgroundColor[2], m_backgroundColor[3]);
        glClear(GL_COLOR_BUFFER_BIT);
        s_shader->use();
        s_vbo->draw();

        
#ifdef ENGINE_DEBUG
        ImGuiIO& io = ImGui::GetIO();
        io.DisplaySize.x = static_cast<float>(get_width());
        io.DisplaySize.y = static_cast<float>(get_height());

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("BackGroundColorWindow");
        ImGui::ColorEdit4("Background Color", m_backgroundColor);
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#endif

        glfwSwapBuffers(m_pWindow);
        glfwPollEvents();
    }
}