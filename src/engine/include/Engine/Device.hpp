#ifndef MINECRAFTBUTACTUALLYGOOD_DEVICE_HPP
#define MINECRAFTBUTACTUALLYGOOD_DEVICE_HPP

//STD
#include <string>
#include <vector>

//INNER
#include <Engine/Window.hpp>

namespace Engine {

    struct SwapChainSupportDetails {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };

    struct QueueFamilyIndices {
        uint32_t graphicsFamily;
        uint32_t presentFamily;
        bool graphicsFamilyHasValue = false;
        bool presentFamilyHasValue = false;
        bool isComplete() {
            return graphicsFamilyHasValue && presentFamilyHasValue;
        }
    };

    class Device {
    public:
#ifdef NDEBUG
        const bool mEnableValidationLayers = false;
#else
        const bool mEnableValidationLayers = true;
#endif

        Device();
        ~Device();

        // Not copyable or movable
        Device(const Device &) = delete;
        void operator=(const Device &) = delete;
        Device(Device &&) = delete;
        Device &operator=(Device &&) = delete;

        VkCommandPool getCommandPool() { return mCommandPool; }
        VkDevice device() { return mDevice; }
        VkSurfaceKHR surface() { return mSurface; }
        VkQueue graphicsQueue() { return mGraphicsQueue; }
        VkQueue presentQueue() { return mPresentQueue; }

        SwapChainSupportDetails getSwapChainSupport() {
            return querySwapChainSupport(mPhysicalDevice);
        }

        uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

        QueueFamilyIndices findPhysicalQueueFamilies() {
            return findQueueFamilies(mPhysicalDevice);
        }
        VkFormat findSupportedFormat(const std::vector<VkFormat> &candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

        // Buffer Helper Functions
        void createBuffer(
                VkDeviceSize size,
                VkBufferUsageFlags usage,
                VkMemoryPropertyFlags properties,
                VkBuffer &buffer,
                VkDeviceMemory &bufferMemory);

        VkCommandBuffer beginSingleTimeCommands();
        void endSingleTimeCommands(VkCommandBuffer commandBuffer);
        void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
        void copyBufferToImage( VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t layerCount);

        void createImageWithInfo(
                const VkImageCreateInfo &imageInfo,
                VkMemoryPropertyFlags properties,
                VkImage &image,
                VkDeviceMemory &imageMemory);

        VkPhysicalDeviceProperties mProperties;

    private:
        void createInstance();
        void setupDebugMessenger();
        void createSurface();
        void pickPhysicalDevice();
        void createLogicalDevice();
        void createCommandPool();

        // helper functions
        bool isDeviceSuitable(VkPhysicalDevice device);
        std::vector<const char *> getRequiredExtensions();
        bool checkValidationLayerSupport();
        QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
        void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);
        void hasGflwRequiredInstanceExtensions();
        bool checkDeviceExtensionSupport(VkPhysicalDevice device);
        SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);

        VkInstance mInstance;
        VkDebugUtilsMessengerEXT mDebugMessenger;
        VkPhysicalDevice mPhysicalDevice = VK_NULL_HANDLE;
        VkCommandPool mCommandPool;

        VkDevice mDevice;
        VkSurfaceKHR mSurface;
        VkQueue mGraphicsQueue;
        VkQueue mPresentQueue;

        const std::vector<const char *> mValidationLayers = {"VK_LAYER_KHRONOS_validation"};
        const std::vector<const char *> mDeviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

    };
}


#endif //MINECRAFTBUTACTUALLYGOOD_DEVICE_HPP
