#ifndef MINECRAFTBUTACTUALLYGOOD_SHADERLOADER_HPP
#define MINECRAFTBUTACTUALLYGOOD_LAUNCHER_HPP

class GameEngine {
public:
    static GameEngine& getEngine()
    {
        static GameEngine instance; // Guaranteed to be destroyed.
        return instance;
    }
private:
    GameEngine() = default;

public:
    GameEngine(GameEngine const&) = delete;
    void operator=(GameEngine const&)  = delete;

    void init();
};


#endif //MINECRAFTBUTACTUALLYGOOD_SHADERLOADER_HPP
