#ifndef WINDOW_HPP
#define WINDOW_HPP

struct GLFWwindow;

namespace EverEngine {

    class Window
    {
    public:
        Window(const char* title, const unsigned int width,
            const unsigned int height);
        ~Window();

        Window(const Window&) = delete;
        Window(Window&&) = delete;
        Window& operator=(const Window&) = delete;
        Window& operator=(Window&&) = delete;

        void on_update();
        unsigned int get_width() const 
        {
            return m_width;
        }
        unsigned int get_height() const 
        {
            return m_height;
        }

    private:
        int init();
        void shutdown();

        GLFWwindow* m_pWindow;
        const char* m_title;
        unsigned int m_width;
        unsigned int m_height;
    };
    
}

#endif // !WINDOW_HPP