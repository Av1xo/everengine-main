#ifndef EVENT_HPP
#define EVENT_HPP

#include <functional>
#include <vector>
#include <array>
#include <queue>
#include <mutex>
#include <memory>

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
        template<typename EventT>
        void add_event_listener(std::function<void(EventT&)> callback)
        {
            const size_t index = static_cast<size_t>(EventT::type);

            auto baseCallback = [func = std::move(callback)](BaseEvent& e)
            {
                func(static_cast<EventT&>(e));
            };

            m_eventCallbacks[index].push_back(std::move(baseCallback));
        }

        void post_event(std::unique_ptr<BaseEvent> event)
        {
            std::lock_guard<std::mutex> lock(m_queueMutex);
            m_queue.push(std::move(event));
        }

        void process_events()
        {
            std::queue<std::unique_ptr<BaseEvent>> localQueue;

            {
                std::lock_guard<std::mutex> lock(m_queueMutex);
                std::swap(localQueue, m_queue);
            }
            
            while (!localQueue.empty())
            {
                auto& event = *localQueue.front();
                dispatch(event);
                localQueue.pop();
            }
        }

        void dispatch(BaseEvent& event)
        {
            const size_t index = static_cast<size_t>(event.get_type());

            for (auto& callback : m_eventCallbacks[index])
            {
                callback(event);
            }
        }

    private:
        std::array<std::vector<std::function<void(BaseEvent&)>>,
                static_cast<size_t>(EventType::EventCount)> m_eventCallbacks {};
        std::queue<std::unique_ptr<BaseEvent>> m_queue;
        std::mutex m_queueMutex;
    };

    struct EventWindowClose : public BaseEvent
    {
        static inline const EventType type = EventType::WindowClose;

        EventType get_type() const override { return type; }
    };

    struct EventMouseMoved : public BaseEvent
    {
        static inline const EventType type = EventType::MouseMoved;

        double x;
        double y;

        EventMouseMoved(double nx, double ny)
            : x(nx), y(ny) {}

        EventType get_type() const override { return type; }
    };

    struct EventWindowResize : public BaseEvent
    {
        static inline const EventType type = EventType::WindowResize;

        unsigned int width;
        unsigned int height;

        EventWindowResize(unsigned int w, unsigned int h)
            : width(w), height(h) {}

        EventType get_type() const override { return type; }
    };
}

#endif // EVENT_HPP
