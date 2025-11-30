#include <iostream>
#include <memory>

#include <EverEngineCore/Application.hpp>

class Editor : public EverEngine::Application
{
public:
    int frame = 0;

    virtual void on_update() override 
    {
        std::cout << "Update frame: " <<  frame++ << std::endl;
    }
};


int main()
{
    std::cout << "Hello from EverEngineEditor" << std::endl;
    
    auto editor = std::make_unique<Editor>();

    int returnCode = editor->start(1024, 768, "Test Application class");

    std::cin.get();
    
    return returnCode;
}