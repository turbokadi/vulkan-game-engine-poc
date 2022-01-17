// STD
#include <iostream>
#include <stdexcept>
#include <cstdlib>

//INNER
#include <GameCommon/GameLoader.hpp>
#include <GameCommon/Logger.hpp>

int main() {

    auto game{Game::GameLoader()};

    try {
        Logger::Info("Launch Game");
        game.run();
        Logger::Info("Finish Game");
    }
    catch (const std::exception& e) {
        Logger::Error( OSS << "Game End unexpectedly : " << e.what());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
