#ifndef MINECRAFTBUTACTUALLYGOOD_WINDOW_HPP
#define MINECRAFTBUTACTUALLYGOOD_WINDOW_HPP

//STD
#include <string>

//GLFW
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define G_WINDOW Engine::Window::getWindow()

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

        uint32_t mWidth;
        uint32_t mHeight;

        std::string mName;
        GLFWwindow* mWindow;

    public:
        Window(Window const&) = delete;
        void operator=(Window const&) = delete;

        uint32_t getWidth() { return mWidth; }
        uint32_t getHeight() { return mHeight; }

        bool shouldClose();
        void createWindowSurface(VkInstance pInstance, VkSurfaceKHR* pSurface);

        VkExtent2D getExtent() { return {mWidth, mHeight}; };

        void open();
        void setupWindow(uint32_t pWidth, uint32_t pHeight,const std::string& pName);
    };
}


#endif //MINECRAFTBUTACTUALLYGOOD_WINDOW_HPP
