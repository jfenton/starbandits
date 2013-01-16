#pragma once

#include "pixelboost/framework/engine.h"

namespace pb
{
    class Shader;
}

class GameScreen;

class Game : public pb::Engine
{
public:
    Game(void* viewController);
    ~Game();
    
    static Game* Instance();
    
    virtual void Update(float time);
    virtual void Render();
    
    GameScreen* GetGameScreen();
    
    pb::Shader* GetLitShader();
    
private:
    GameScreen* _GameScreen;
};
