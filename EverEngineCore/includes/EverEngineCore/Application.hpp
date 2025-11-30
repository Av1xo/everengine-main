#ifndef APPLICATION_HPP
#define APPLICATION_HPP

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
    };

}

#endif // !APPLICATION_HPP