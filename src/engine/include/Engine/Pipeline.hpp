#ifndef MINECRAFTBUTACTUALLYGOOD_PIPELINE_HPP
#define MINECRAFTBUTACTUALLYGOOD_PIPELINE_HPP

//STD
#include <string>
#include <vector>

//INNER
#include <Engine/Device.hpp>

namespace Engine {
    struct PipelineConfigInfo {
        VkViewport viewport;
        VkRect2D scissor;
        VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
        VkPipelineRasterizationStateCreateInfo rasterizationInfo;
        VkPipelineMultisampleStateCreateInfo multisampleInfo;
        VkPipelineColorBlendAttachmentState colorBlendAttachment;
        VkPipelineColorBlendStateCreateInfo colorBlendInfo;
        VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
        VkPipelineLayout pipelineLayout = nullptr;
        VkRenderPass renderPass = nullptr;
        uint32_t subpass = 0;
    };

    class Pipeline {
    public:
        Pipeline(Device& pDevice, const PipelineConfigInfo& pConfigInfo, const std::string& pVertFilePath, const std::string& pFragmentFilePath);
        ~Pipeline();

        Pipeline(const Pipeline&) = delete;
        Pipeline& operator=(const Pipeline&) = delete;

        void bind(VkCommandBuffer pCommandBuffer);
        static PipelineConfigInfo defaultPipelineConfigInfo(uint32_t pWidth, uint32_t pHeight);

    private:
        static std::vector<char> readFile(const std::string& pFilePath);

        void createGraphicsPipeline(const std::string& pVertFilePath, const std::string& pFragmentFilePath, const PipelineConfigInfo& pConfigInfo);

        void createShaderModule(const std::vector<char>& pCode, VkShaderModule* pShaderModule);

        Engine::Device& mDevice; // Unsafe
        VkPipeline mGraphicsPipeline;
        VkShaderModule mVertShaderModule;
        VkShaderModule mFragmentShaderModule;
    };
}

#endif //MINECRAFTBUTACTUALLYGOOD_PIPELINE_HPP
