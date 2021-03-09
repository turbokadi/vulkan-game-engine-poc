//STD
#include <stdexcept>
#include <array>

//INNER
#include <Engine/GameEngine.hpp>
#include <Engine/Pipeline.hpp>

GameEngine::GameEngine() {
    createPipelineLayout();
    createPipeline();
    createCommandBuffers();
}

GameEngine::~GameEngine() {
    vkDestroyPipelineLayout(mDevice.device(), mPipelineLayout, nullptr);
}

void GameEngine::init() {
}

void GameEngine::createPipelineLayout() {
    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 0;
    pipelineLayoutInfo.pSetLayouts = nullptr;
    pipelineLayoutInfo.pushConstantRangeCount = 0;
    pipelineLayoutInfo.pPushConstantRanges = nullptr;

    if (vkCreatePipelineLayout(mDevice.device(), &pipelineLayoutInfo, nullptr, &mPipelineLayout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create pipeline layout !");
    }
}

void GameEngine::createPipeline() {
    auto pipelineConfig = Engine::Pipeline::defaultPipelineConfigInfo(mSwapChain.width(), mSwapChain.height());
    pipelineConfig.renderPass = mSwapChain.getRenderPass();
    pipelineConfig.pipelineLayout = mPipelineLayout;
    mPipeline = std::make_unique<Engine::Pipeline>(
            mDevice,
            pipelineConfig,
            "shaders/Triangle.vert.spv",
            "shaders/Triangle.frag.spv"
            );
}

void GameEngine::createCommandBuffers() {
    mCommandBuffers.resize(mSwapChain.imageCount());
    VkCommandBufferAllocateInfo allocateInfo{};
    allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocateInfo.commandPool = mDevice.getCommandPool();
    allocateInfo.commandBufferCount = static_cast<uint32_t>(mCommandBuffers.size());

    if (vkAllocateCommandBuffers(mDevice.device(), &allocateInfo, mCommandBuffers.data()) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate command buffers !");
    }

    for (int i=0; mCommandBuffers.size(); i++) {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if ( vkBeginCommandBuffer(mCommandBuffers[i], &beginInfo) != VK_SUCCESS) {
            throw std::runtime_error("failed to begin recording command buffer !");
        }

        VkRenderPassBeginInfo renderPassBeginInfo{};
        renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassBeginInfo.renderPass = mSwapChain.getRenderPass();
        renderPassBeginInfo.framebuffer = mSwapChain.getFrameBuffer(i);

        renderPassBeginInfo.renderArea.offset = {0,0};
        renderPassBeginInfo.renderArea.extent = mSwapChain.getSwapChainExtent();

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = {{1.0f,1.0f,1.0f,1.0f}};
        clearValues[1].depthStencil = {1.0f, 0};
        renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassBeginInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(mCommandBuffers[i], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

        mPipeline->bind(mCommandBuffers[i]);
        vkCmdDraw(mCommandBuffers[i], 3, 1, 0, 0);

        vkCmdEndRenderPass(mCommandBuffers[i]);
        if (vkEndCommandBuffer(mCommandBuffers[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to record command buffer !");
        }
    }
}

void GameEngine::drawFrame() {

}
