//STD
#include <stdexcept>

//INNER
#include "Engine/Window.hpp"
#include <GameCommon/Logger.hpp>

namespace Engine {
    Window::~Window() {
        glfwDestroyWindow(mWindow);
        glfwTerminate();
    }

    void Window::open() {
        if (mWindow == nullptr) {
            glfwInit();
            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
            glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

            mWindow = glfwCreateWindow(static_cast<int>(mWidth),
                                       static_cast<int>(mHeight),
                                       mName.c_str(), nullptr,
                                       nullptr);
        }
        else {
            Logger::Error("Window already opened !");
        }
    }

    bool Window::shouldClose() {
        return glfwWindowShouldClose(mWindow);;
    }

    void Window::setupWindow(unsigned int pWidth, unsigned int pHeight, const std::string &pName) {
        mHeight = pHeight;
        mWidth = pWidth;
        mName = pName;
    }

    void Window::createWindowSurface(VkInstance pInstance, VkSurfaceKHR *pSurface) {
        if (glfwCreateWindowSurface(pInstance, mWindow, nullptr, pSurface) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create window surface !");
        }
    }
}