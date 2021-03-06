//INNER
#include <Engine/GameEngine.hpp>
#include <Engine/Pipeline.hpp>

void GameEngine::init() {
    Engine::Pipeline pipeline("shaders/Triangle.vert.spv","shaders/Triangle.frag.spv");
}
