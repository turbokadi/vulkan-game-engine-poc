#ifndef MINECRAFTBUTACTUALLYGOOD_SHADERLOADER_HPP
#define MINECRAFTBUTACTUALLYGOOD_LAUNCHER_HPP

//STD
#include <memory>
#include <vector>

//INNER
#include <Engine/Device.hpp>
#include <Engine/Pipeline.hpp>
#include <Engine/SwapChain.hpp>

class GameEngine {
public:
    static GameEngine& getEngine()
    {
        static GameEngine instance; // Guaranteed to be destroyed.
        return instance;
    }
private:
    GameEngine();
    ~GameEngine();

    void createPipelineLayout();
    void createPipeline();
    void createCommandBuffers();
    void drawFrame();

    Engine::Device mDevice;
    Engine::SwapChain mSwapChain{mDevice, Engine::Window::getWindow().getExtent()};
    std::unique_ptr<Engine::Pipeline> mPipeline;
    VkPipelineLayout mPipelineLayout;
    std::vector<VkCommandBuffer> mCommandBuffers;

public:
    GameEngine(GameEngine const&) = delete;
    void operator=(GameEngine const&)  = delete;

    void init();
};


#endif //MINECRAFTBUTACTUALLYGOOD_SHADERLOADER_HPP
