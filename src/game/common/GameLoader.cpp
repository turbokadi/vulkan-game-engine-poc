//INNER
#include <GameCommon/GameLoader.hpp>
#include <GameCommon/Config.hpp>

namespace Game {
    GameLoader::GameLoader() {
        Engine::Window::getWindow().setupWindow(800, 600, GAME_NAME);
        Engine::Window::getWindow().open();
        GameEngine::getEngine().init();
    }

    GameLoader::~GameLoader() {
    }

    void GameLoader::run() {
        while (!Engine::Window::getWindow().shouldClose()) {
            glfwPollEvents();
        }
    }
}
