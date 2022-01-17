//STD
#include <stdexcept>
#include <array>

//INNER
#include <Engine/GameEngine.hpp>
#include <Engine/Pipeline.hpp>

GameEngine::GameEngine() {
    loadModels();
    createPipelineLayout();
    createPipeline();
    createCommandBuffers();
}

GameEngine::~GameEngine() {
    vkDestroyPipelineLayout(mDevice.device(), mPipelineLayout, nullptr);
}

void GameEngine::init() {
    while (!G_WINDOW.shouldClose()) {
        glfwPollEvents();
        drawFrame();
    }

    vkDeviceWaitIdle(mDevice.device());
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

    for (size_t i=0; i < mCommandBuffers.size(); i++) {
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
        mModel->bind(mCommandBuffers[i]);
        mModel->draw(mCommandBuffers[i]);

        vkCmdEndRenderPass(mCommandBuffers[i]);
        if (vkEndCommandBuffer(mCommandBuffers[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to record command buffer !");
        }
    }
}

void GameEngine::drawFrame() {
    uint32_t imageIndex;
    auto result = mSwapChain.acquireNextImage(&imageIndex);

    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) { // TODO resize event
        throw std::runtime_error("failed to acquire swap chain image !");
    }

    result = mSwapChain.submitCommandBuffers(&mCommandBuffers[imageIndex], &imageIndex);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to present swap chain image !");
    }
}

void GameEngine::loadModels() {
    std::vector<Engine::Model::Vertex> vertices {
            {{0.0f, -0.5f}},
            {{0.5f, 0.5f}},
            {{-0.5f, 0.5f}},
    };
    mModel = std::make_unique<Engine::Model>(mDevice,vertices);
}
