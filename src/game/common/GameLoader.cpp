//INNER
#include <GameCommon/GameLoader.hpp>
#include <GameCommon/Config.hpp>

namespace Game {
    GameLoader::GameLoader() {
        G_WINDOW.setupWindow(800, 600, GAME_NAME);
        G_WINDOW.open();
    }

    GameLoader::~GameLoader() {
    }

    void GameLoader::run() {
        G_ENGINE.init();
    }
}
