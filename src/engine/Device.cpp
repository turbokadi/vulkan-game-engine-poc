//STD
#include <cstring>
#include <iostream>
#include <set>
#include <unordered_set>

// INNER
#include "Engine/Device.hpp"
#include <GameCommon/Logger.hpp>
#include <GameCommon/Config.hpp>

namespace Engine {
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback( VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                         VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                         const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                         void *pUserData) {
        Logger::Debug( OSS << "VK_DEBUG -> " << pCallbackData->pMessage);
        return VK_FALSE;
    }

    VkResult CreateDebugUtilsMessengerEXT( VkInstance instance,
                                           const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
                                           const VkAllocationCallbacks *pAllocator,
                                           VkDebugUtilsMessengerEXT *pDebugMessenger) {
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance,"vkCreateDebugUtilsMessengerEXT");
        if (func != nullptr) {
            return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
        }
        else {
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
    }

    void DestroyDebugUtilsMessengerEXT( VkInstance instance,
                                        VkDebugUtilsMessengerEXT debugMessenger,
                                        const VkAllocationCallbacks *pAllocator) {
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
                instance,
                "vkDestroyDebugUtilsMessengerEXT");
        if (func != nullptr) {
            func(instance, debugMessenger, pAllocator);
        }
    }

    Device::Device() {
        createInstance();
        setupDebugMessenger();
        createSurface();
        pickPhysicalDevice();
        createLogicalDevice();
        createCommandPool();
    }

    Device::~Device() {
        vkDestroyCommandPool(mDevice, mCommandPool, nullptr);
        vkDestroyDevice(mDevice, nullptr);

        if (mEnableValidationLayers) {
            DestroyDebugUtilsMessengerEXT(mInstance, mDebugMessenger, nullptr);
        }

        vkDestroySurfaceKHR(mInstance, mSurface, nullptr);
        vkDestroyInstance(mInstance, nullptr);
    }

    void Device::createInstance() {
        if (mEnableValidationLayers && !checkValidationLayerSupport()) {
            throw std::runtime_error("validation layers requested, but not available!");
        }

        VkApplicationInfo appInfo = {};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = GAME_NAME;
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = GAME_ENGINE_NAME;
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;

        VkInstanceCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;

        auto extensions = getRequiredExtensions();
        createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        createInfo.ppEnabledExtensionNames = extensions.data();

        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
        if (mEnableValidationLayers) {
            createInfo.enabledLayerCount = static_cast<uint32_t>(mValidationLayers.size());
            createInfo.ppEnabledLayerNames = mValidationLayers.data();

            populateDebugMessengerCreateInfo(debugCreateInfo);
            createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT *)&debugCreateInfo;
        }
        else {
            createInfo.enabledLayerCount = 0;
            createInfo.pNext = nullptr;
        }

        if (vkCreateInstance(&createInfo, nullptr, &mInstance) != VK_SUCCESS) {
            throw std::runtime_error("failed to create mInstance!");
        }

        hasGflwRequiredInstanceExtensions();
    }

    void Device::pickPhysicalDevice() {
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(mInstance, &deviceCount, nullptr);
        if (deviceCount == 0) {
            throw std::runtime_error("failed to find GPUs with Vulkan support!");
        }
        Logger::Info( OSS << "Device count : " << deviceCount );
        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(mInstance, &deviceCount, devices.data());

        for (const auto &device : devices) {
            if (isDeviceSuitable(device)) {
                mPhysicalDevice = device;
                break;
            }
        }

        if (mPhysicalDevice == VK_NULL_HANDLE) {
            throw std::runtime_error("failed to find a suitable GPU!");
        }

        vkGetPhysicalDeviceProperties(mPhysicalDevice, &mProperties);
        Logger::Info( OSS << "Physical device : \033[1;36m" << mProperties.deviceName << "\033[0m" );
    }

    void Device::createLogicalDevice() {
        QueueFamilyIndices indices = findQueueFamilies(mPhysicalDevice);

        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily, indices.presentFamily};

        float queuePriority = 1.0f;
        for (uint32_t queueFamily : uniqueQueueFamilies) {
            VkDeviceQueueCreateInfo queueCreateInfo = {};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queueFamily;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);
        }

        VkPhysicalDeviceFeatures deviceFeatures = {};
        deviceFeatures.samplerAnisotropy = VK_TRUE;

        VkDeviceCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

        createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
        createInfo.pQueueCreateInfos = queueCreateInfos.data();

        createInfo.pEnabledFeatures = &deviceFeatures;
        createInfo.enabledExtensionCount = static_cast<uint32_t>(mDeviceExtensions.size());
        createInfo.ppEnabledExtensionNames = mDeviceExtensions.data();

        // might not really be necessary anymore because device specific validation layers
        // have been deprecated
        if (mEnableValidationLayers) {
            createInfo.enabledLayerCount = static_cast<uint32_t>(mValidationLayers.size());
            createInfo.ppEnabledLayerNames = mValidationLayers.data();
        } else {
            createInfo.enabledLayerCount = 0;
        }

        if (vkCreateDevice(mPhysicalDevice, &createInfo, nullptr, &mDevice) != VK_SUCCESS) {
            throw std::runtime_error("failed to create logical device!");
        }

        vkGetDeviceQueue(mDevice, indices.graphicsFamily, 0, &mGraphicsQueue);
        vkGetDeviceQueue(mDevice, indices.presentFamily, 0, &mPresentQueue);
    }

    void Device::createCommandPool() {
        QueueFamilyIndices queueFamilyIndices = findPhysicalQueueFamilies();

        VkCommandPoolCreateInfo poolInfo = {};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily;
        poolInfo.flags =
                VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

        if (vkCreateCommandPool(mDevice, &poolInfo, nullptr, &mCommandPool) != VK_SUCCESS) {
            throw std::runtime_error("failed to create command pool!");
        }
    }

    void Device::createSurface() {
        G_WINDOW.createWindowSurface(mInstance, &mSurface);
    }

    bool Device::isDeviceSuitable(VkPhysicalDevice device) {
        QueueFamilyIndices indices = findQueueFamilies(device);

        bool extensionsSupported = checkDeviceExtensionSupport(device);

        bool swapChainAdequate = false;
        if (extensionsSupported) {
            SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
            swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
        }

        VkPhysicalDeviceFeatures supportedFeatures;
        vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

        return indices.isComplete() && extensionsSupported && swapChainAdequate &&
               supportedFeatures.samplerAnisotropy;
    }

    void Device::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo) {
        createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                     VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                     VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = debugCallback;
        createInfo.pUserData = nullptr;  // Optional
    }

    void Device::setupDebugMessenger() {
        if (mEnableValidationLayers) {
            VkDebugUtilsMessengerCreateInfoEXT createInfo;
            populateDebugMessengerCreateInfo(createInfo);
            if (CreateDebugUtilsMessengerEXT(mInstance, &createInfo, nullptr, &mDebugMessenger) != VK_SUCCESS) {
                throw std::runtime_error("failed to set up debug messenger!");
            }
        };
    }

    bool Device::checkValidationLayerSupport() {
        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        for (const char *layerName : mValidationLayers) {
            bool layerFound = false;

            for (const auto &layerProperties : availableLayers) {
                if (strcmp(layerName, layerProperties.layerName) == 0) {
                    layerFound = true;
                    break;
                }
            }

            if (!layerFound) {
                return false;
            }
        }

        return true;
    }

    std::vector<const char *> Device::getRequiredExtensions() {
        uint32_t glfwExtensionCount = 0;
        const char **glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector<const char *> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

        if (mEnableValidationLayers) {
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }

        return extensions;
    }

    void Device::hasGflwRequiredInstanceExtensions() {
        uint32_t extensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
        std::vector<VkExtensionProperties> extensions(extensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

        std::unordered_set<std::string> available;
        std::ostringstream extensionListStr;
        for (const auto &extension : extensions) {
            extensionListStr << "\t" << extension.extensionName << "\n";
            available.insert(extension.extensionName);
        }
        Logger::Info( "available extensions : \n" + extensionListStr.str());

        extensionListStr.flush();
        auto requiredExtensions = getRequiredExtensions();
        for (const auto &required : requiredExtensions) {
            extensionListStr << "\t" << required << "\n";
            if (available.find(required) == available.end()) {
                throw std::runtime_error("Missing required glfw extension");
            }
        }
        Logger::Info( "required extensions : \n" + extensionListStr.str());
    }

    bool Device::checkDeviceExtensionSupport(VkPhysicalDevice device) {
        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(
                device,
                nullptr,
                &extensionCount,
                availableExtensions.data());

        std::set<std::string> requiredExtensions(mDeviceExtensions.begin(), mDeviceExtensions.end());

        for (const auto &extension : availableExtensions) {
            requiredExtensions.erase(extension.extensionName);
        }

        return requiredExtensions.empty();
    }

    QueueFamilyIndices Device::findQueueFamilies(VkPhysicalDevice device) {
        QueueFamilyIndices indices;

        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

        int i = 0;
        for (const auto &queueFamily : queueFamilies) {
            if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                indices.graphicsFamily = i;
                indices.graphicsFamilyHasValue = true;
            }
            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, mSurface, &presentSupport);
            if (queueFamily.queueCount > 0 && presentSupport) {
                indices.presentFamily = i;
                indices.presentFamilyHasValue = true;
            }
            if (indices.isComplete()) {
                break;
            }

            i++;
        }

        return indices;
    }

    SwapChainSupportDetails Device::querySwapChainSupport(VkPhysicalDevice device) {
        SwapChainSupportDetails details;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, mSurface, &details.capabilities);

        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, mSurface, &formatCount, nullptr);

        if (formatCount != 0) {
            details.formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, mSurface, &formatCount, details.formats.data());
        }

        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, mSurface, &presentModeCount, nullptr);

        if (presentModeCount != 0) {
            details.presentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(
                    device,
                    mSurface,
                    &presentModeCount,
                    details.presentModes.data());
        }
        return details;
    }

    VkFormat Device::findSupportedFormat(
            const std::vector<VkFormat> &candidates, VkImageTiling tiling, VkFormatFeatureFlags features) {
        for (VkFormat format : candidates) {
            VkFormatProperties props;
            vkGetPhysicalDeviceFormatProperties(mPhysicalDevice, format, &props);

            if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
                return format;
            } else if (
                    tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
                return format;
            }
        }
        throw std::runtime_error("failed to find supported format!");
    }

    uint32_t Device::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(mPhysicalDevice, &memProperties);
        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
            if ((typeFilter & (1 << i)) &&
                (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
                return i;
            }
        }

        throw std::runtime_error("failed to find suitable memory type!");
    }

    void Device::createBuffer(  VkDeviceSize size,
                                VkBufferUsageFlags usage,
                                VkMemoryPropertyFlags properties,
                                VkBuffer &buffer,
                                VkDeviceMemory &bufferMemory) {
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateBuffer(mDevice, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
            throw std::runtime_error("failed to create vertex buffer!");
        }

        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(mDevice, buffer, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

        if (vkAllocateMemory(mDevice, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate vertex buffer memory!");
        }

        vkBindBufferMemory(mDevice, buffer, bufferMemory, 0);
    }

    VkCommandBuffer Device::beginSingleTimeCommands() {
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = mCommandPool;
        allocInfo.commandBufferCount = 1;

        VkCommandBuffer commandBuffer;
        vkAllocateCommandBuffers(mDevice, &allocInfo, &commandBuffer);

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(commandBuffer, &beginInfo);
        return commandBuffer;
    }

    void Device::endSingleTimeCommands(VkCommandBuffer commandBuffer) {
        vkEndCommandBuffer(commandBuffer);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;

        vkQueueSubmit(mGraphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
        vkQueueWaitIdle(mGraphicsQueue);

        vkFreeCommandBuffers(mDevice, mCommandPool, 1, &commandBuffer);
    }

    void Device::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {
        VkCommandBuffer commandBuffer = beginSingleTimeCommands();

        VkBufferCopy copyRegion{};
        copyRegion.srcOffset = 0;  // Optional
        copyRegion.dstOffset = 0;  // Optional
        copyRegion.size = size;
        vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

        endSingleTimeCommands(commandBuffer);
    }

    void Device::copyBufferToImage( VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t layerCount) {
        VkCommandBuffer commandBuffer = beginSingleTimeCommands();

        VkBufferImageCopy region{};
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;

        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = layerCount;

        region.imageOffset = {0, 0, 0};
        region.imageExtent = {width, height, 1};

        vkCmdCopyBufferToImage(
                commandBuffer,
                buffer,
                image,
                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                1,
                &region);
        endSingleTimeCommands(commandBuffer);
    }

    void Device::createImageWithInfo( const VkImageCreateInfo &imageInfo,
                                      VkMemoryPropertyFlags properties,
                                      VkImage &image,
                                      VkDeviceMemory &imageMemory) {
        if (vkCreateImage(mDevice, &imageInfo, nullptr, &image) != VK_SUCCESS) {
            throw std::runtime_error("failed to create image!");
        }

        VkMemoryRequirements memRequirements;
        vkGetImageMemoryRequirements(mDevice, image, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

        if (vkAllocateMemory(mDevice, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate image memory!");
        }

        if (vkBindImageMemory(mDevice, image, imageMemory, 0) != VK_SUCCESS) {
            throw std::runtime_error("failed to bind image memory!");
        }
    }
}