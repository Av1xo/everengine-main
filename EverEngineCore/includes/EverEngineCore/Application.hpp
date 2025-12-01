#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include <memory>

namespace EverEngine 
{
    class Application
    {
    private:
        
    public:
        Application(/* args */);
        virtual ~Application();

        Application(const Application&) = delete;
        Application(Application&&) = delete;
        Application& operator=(const Application&) = delete;
        Application& operator=(Application&&) = delete;

        virtual int start(unsigned window_width, unsigned int window_height, const char* title);

        virtual void on_update() {};
    
    private:
        std::unique_ptr<class Window> m_pWindow;
    };

}

#endif // !APPLICATION_HPP