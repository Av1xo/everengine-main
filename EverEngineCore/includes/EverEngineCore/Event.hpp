#ifndef EVENT_HPP
#define EVENT_HPP

#include <functional>

namespace EverEngine
{
    enum class EventType
    {
        WindowResize = 0,
        WindowClose,

        KeyPressed,
        KeyReleased,

        MouseButtonPressed,
        MouseButtonReleased,
        MouseWheelScroll,
        MouseMoved,

        EventCount,
    };

    struct BaseEvent
    {
        virtual ~BaseEvent() = default;
        virtual EventType get_type() const = 0;
    };

    class EventDispatcher
    {
    public:
        template<typename EventType>
        void add_event_listener(std::function<void(EventType&)> callback)
        {
            auto baseCallback = [func = std::move(callback)](BaseEvent& e)
            {
                if (e.get_type() == EventType::type)
                {
                    func(static_cast<EventType&>(e));
                }
            };
            m_eventCallbacks[static_cast<size_t>(EventType::type)] = std::move(baseCallback);
        }

        void dispatch(BaseEvent& event)
        {
            auto& callback = m_eventCallbacks[static_cast<size_t>(event.get_type())];
            if (callback)
            {
                callback(event);
            }
        }

    private:
        std::array<std::function<void(BaseEvent&)>, static_cast<size_t>(EventType::EventCount)> m_eventCallbacks;
    };

    struct EventWindowClose : public BaseEvent
    {
        virtual EventType get_type() const override
        {
            return type;
        }

        static const EventType type = EventType::WindowClose;
    };

    struct EventMouseMoved : public BaseEvent
    {
        EventMouseMoved(const double new_x, const double new_y)
            : x(new_x)
            , y(new_y)
        {}

        virtual EventType get_type() const override
        {
            return type;
        }

        double x;
        double y;

        static const EventType type = EventType::MouseMoved;
    };

    struct EventWindowResize : public BaseEvent
    {
        EventWindowResize(const unsigned int new_width, const unsigned int new_heigh)
            : width(new_width)
            , height(new_heigh)
        {}

        virtual EventType get_type() const override
        {
            return type;
        }

        unsigned int width;
        unsigned int height;

        static const EventType type = EventType::WindowResize;
    };
}

#endif // !EVENT_HPP
