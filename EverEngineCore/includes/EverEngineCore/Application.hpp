#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include "EverEngineCore/Event.hpp"

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
        std::unique_ptr<class Renderer> m_Renderer;

        EventDispatcher m_event_dispatcher;
        bool m_bCloseWindow = false;
    };

}

#endif // !APPLICATION_HPP