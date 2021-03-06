#ifndef MINECRAFTBUTACTUALLYGOOD_WINDOW_HPP
#define MINECRAFTBUTACTUALLYGOOD_WINDOW_HPP

//STD
#include <string>

//GLFW
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace Engine {
    class Window {
    public:
        static Window& getWindow()
        {
            static Window instance; // Guaranteed to be destroyed.
            return instance;
        }
    private:
        Window() = default;
        ~Window();

        unsigned int mWidth;
        unsigned int mHeight;

        std::string mName;
        GLFWwindow* mWindow;

    public:
        Window(Window const&) = delete;
        void operator=(Window const&) = delete;

        bool shouldClose();
        void createWindowSurface(VkInstance pInstance, VkSurfaceKHR* pSurface);

        void open();
        void setupWindow(unsigned int pWidth, unsigned int pHeight,const std::string& pName);
    };
}


#endif //MINECRAFTBUTACTUALLYGOOD_WINDOW_HPP
