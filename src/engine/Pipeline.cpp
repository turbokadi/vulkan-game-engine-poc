#include <Engine/Pipeline.hpp>

//STD
#include <fstream>
#include <cassert>

//INNER
#include <GameCommon/Logger.hpp>
#include <Engine/Device.hpp>

namespace Engine {
    Pipeline::Pipeline(Device& pDevice, const PipelineConfigInfo& pConfigInfo, const std::string& pVertFilePath, const std::string& pFragmentFilePath) : mDevice(pDevice) {
        createGraphicsPipeline(pVertFilePath, pFragmentFilePath, pConfigInfo);
    }

    Pipeline::~Pipeline() {
        vkDestroyShaderModule(mDevice.device(), mVertShaderModule, nullptr);
        vkDestroyShaderModule(mDevice.device(), mFragmentShaderModule, nullptr);
        vkDestroyPipeline(mDevice.device(), mGraphicsPipeline, nullptr);
    }

    std::vector<char> Pipeline::readFile(const std::string& pFilePath) {
        std::fstream file;
        file.open(pFilePath, std::ios::ate | std::ios::binary | std::ios::in);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open file : " + pFilePath);
        }

        auto fileSize{static_cast<size_t>(file.tellg())};
        std::vector<char> buffer(fileSize);

        file.seekg(0);
        file.read(buffer.data(), fileSize);
        file.close();

        return buffer;
    }

    void Pipeline::createGraphicsPipeline(const std::string &pVertFilePath, const std::string &pFragmentFilePath, const PipelineConfigInfo& pConfigInfo) {
        auto vertCode{readFile(pVertFilePath)};
        auto fragCode{readFile(pFragmentFilePath)};

        assert(pConfigInfo.pipelineLayout != VK_NULL_HANDLE && "Cannot create graphics pipeline : No pipelineLayout provided in pConfigInfo");
        assert(pConfigInfo.renderPass != VK_NULL_HANDLE && "Cannot create graphics pipeline : No renderPass provided in pConfigInfo");
        createShaderModule(vertCode, &mVertShaderModule);
        createShaderModule(fragCode, &mFragmentShaderModule);

        VkPipelineShaderStageCreateInfo shaderStages[2];
        shaderStages[0] = {};
        shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStages[0].pNext = nullptr;
        shaderStages[0].flags = 0;
        shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
        shaderStages[0].module = mVertShaderModule;
        shaderStages[0].pName = "main";
        shaderStages[0].pSpecializationInfo = nullptr;

        shaderStages[1] = {};
        shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStages[1].pNext = nullptr;
        shaderStages[1].flags = 0;
        shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        shaderStages[1].module = mFragmentShaderModule;
        shaderStages[1].pName = "main";
        shaderStages[1].pSpecializationInfo = nullptr;

        VkPipelineVertexInputStateCreateInfo  vertexInputInfo = {};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexBindingDescriptionCount = 0;
        vertexInputInfo.pVertexBindingDescriptions = nullptr;
        vertexInputInfo.vertexAttributeDescriptionCount = 0;
        vertexInputInfo.pVertexAttributeDescriptions = nullptr;

        VkPipelineViewportStateCreateInfo viewportInfo;
        viewportInfo = {};
        viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportInfo.viewportCount = 1;
        viewportInfo.pViewports = &pConfigInfo.viewport;
        viewportInfo.scissorCount = 1;
        viewportInfo.pScissors = &pConfigInfo.scissor;

        VkGraphicsPipelineCreateInfo pipelineInfo = {};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount = 2;
        pipelineInfo.pStages = shaderStages;
        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &pConfigInfo.inputAssemblyInfo;
        pipelineInfo.pViewportState = &viewportInfo;
        pipelineInfo.pRasterizationState = &pConfigInfo.rasterizationInfo;
        pipelineInfo.pMultisampleState = &pConfigInfo.multisampleInfo;
        pipelineInfo.pDepthStencilState = &pConfigInfo.depthStencilInfo;
        pipelineInfo.pColorBlendState = &pConfigInfo.colorBlendInfo;
        pipelineInfo.pDynamicState = nullptr;
        pipelineInfo.layout = pConfigInfo.pipelineLayout;
        pipelineInfo.renderPass = pConfigInfo.renderPass;
        pipelineInfo.subpass = pConfigInfo.subpass;
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
        pipelineInfo.basePipelineIndex = -1;

        if (vkCreateGraphicsPipelines(mDevice.device(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &mGraphicsPipeline) != VK_SUCCESS) {
            throw std::runtime_error("failed to create graphics pipeline");
        }
    }

    PipelineConfigInfo Pipeline::defaultPipelineConfigInfo(uint32_t pWidth, uint32_t pHeight) {
        PipelineConfigInfo configInfo{};

        configInfo.inputAssemblyInfo = {};
        configInfo.inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        configInfo.inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        configInfo.inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

        configInfo.viewport = {};
        configInfo.viewport.x = 0.0f;
        configInfo.viewport.y = 0.0f;
        configInfo.viewport.width = static_cast<float>(pWidth);
        configInfo.viewport.height = static_cast<float>(pHeight);
        configInfo.viewport.minDepth = 0.0f;
        configInfo.viewport.maxDepth = 1.0f;

        configInfo.scissor = {};
        configInfo.scissor.offset = {0, 0};
        configInfo.scissor.extent = {pWidth, pHeight};

        configInfo.rasterizationInfo = {};
        configInfo.rasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        configInfo.rasterizationInfo.depthClampEnable = VK_FALSE;
        configInfo.rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
        configInfo.rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
        configInfo.rasterizationInfo.cullMode = VK_CULL_MODE_NONE;
        configInfo.rasterizationInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
        configInfo.rasterizationInfo.depthBiasEnable = VK_FALSE;
        configInfo.rasterizationInfo.depthBiasConstantFactor = 0.0f;    // Optional
        configInfo.rasterizationInfo.depthBiasClamp = 0.0f;             // Optional
        configInfo.rasterizationInfo.depthBiasSlopeFactor = 0.0f;       // Optional
        configInfo.rasterizationInfo.lineWidth = 1.0f;

        // MSAA
        configInfo.multisampleInfo = {};
        configInfo.multisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        configInfo.multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        configInfo.multisampleInfo.sampleShadingEnable = VK_FALSE;
        configInfo.multisampleInfo.minSampleShading = 1.0f;
        configInfo.multisampleInfo.pSampleMask = nullptr;
        configInfo.multisampleInfo.alphaToCoverageEnable = VK_FALSE;
        configInfo.multisampleInfo.alphaToOneEnable = VK_FALSE;

        configInfo.colorBlendAttachment = {};
        configInfo.colorBlendAttachment.blendEnable = VK_FALSE;
        configInfo.colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
        configInfo.colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
        configInfo.colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
        configInfo.colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        configInfo.colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        configInfo.colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
        configInfo.colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

        configInfo.colorBlendInfo = {};
        configInfo.colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        configInfo.colorBlendInfo.logicOpEnable = VK_FALSE;
        configInfo.colorBlendInfo.logicOp = VK_LOGIC_OP_COPY;
        configInfo.colorBlendInfo.attachmentCount = 1;
        configInfo.colorBlendInfo.pAttachments = &configInfo.colorBlendAttachment;
        configInfo.colorBlendInfo.blendConstants[0] = 0.0f;
        configInfo.colorBlendInfo.blendConstants[1] = 0.0f;
        configInfo.colorBlendInfo.blendConstants[2] = 0.0f;
        configInfo.colorBlendInfo.blendConstants[3] = 0.0f;

        configInfo.depthStencilInfo = {};
        configInfo.depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        configInfo.depthStencilInfo.depthTestEnable = VK_TRUE;
        configInfo.depthStencilInfo.depthWriteEnable = VK_TRUE;
        configInfo.depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS;
        configInfo.depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
        configInfo.depthStencilInfo.stencilTestEnable = VK_FALSE;
        configInfo.depthStencilInfo.front = {};    // Optional
        configInfo.depthStencilInfo.back = {};     // Optional
        configInfo.depthStencilInfo.minDepthBounds = 0.0f;
        configInfo.depthStencilInfo.maxDepthBounds = 1.0f;

        return configInfo;
    }

    void Pipeline::createShaderModule(const std::vector<char> &pCode, VkShaderModule* pShaderModule) {
        VkShaderModuleCreateInfo createInfo = {};
        createInfo.sType =   VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = pCode.size();
        createInfo.pCode = reinterpret_cast<const uint32_t*>(pCode.data());

        if (vkCreateShaderModule(mDevice.device(), &createInfo, nullptr, pShaderModule) != VK_SUCCESS) {
            throw std::runtime_error("failed to create shader module.");
        }
    }

    void Pipeline::bind(VkCommandBuffer pCommandBuffer) {
        if (pCommandBuffer != nullptr) {
            throw std::runtime_error("Empty command Buffer ...");
        }
        vkCmdBindPipeline(pCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mGraphicsPipeline);
    }
}

