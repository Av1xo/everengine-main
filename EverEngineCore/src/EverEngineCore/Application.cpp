#include "EverEngineCore/Application.hpp"
#include "EverEngineCore/Log.hpp"
#include "Window.hpp"
#include "EverEngineCore/Event.hpp"

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

        m_event_dispatcher.add_event_listener<EventMouseMoved>(
            [](EventMouseMoved& event)
            {
                LOG_INFO("EVENT::MOUSE::MOVE({0}x{1})", event.x, event.y);
            }
        );

        m_event_dispatcher.add_event_listener<EventWindowResize>(
            [](EventWindowResize& event)
            {
                LOG_INFO("EVENT::CHANGE::SIZE({0}x{1})", event.width, event.height);
            }
        );
        m_event_dispatcher.add_event_listener<EventWindowClose>(
            [&](EventWindowClose& event){
                LOG_INFO("[WINDOW_CLOSE]");
                m_bCloseWindow = true;
            }
        );

        m_pWindow->set_event_callback(
            [&](std::unique_ptr<BaseEvent> event){
                m_event_dispatcher.post_event(std::move(event));
            }
        );

        while (!m_bCloseWindow)
        {
            m_pWindow->on_update();
            m_event_dispatcher.process_events();
            on_update();
        }
        m_pWindow = nullptr;

        return 0;
    }
}