#pragma once

#include "pixelboost/framework/engine.h"

class GameScreen;

class Game : public pb::Engine
{
public:
    Game(void* viewController);
    ~Game();
    
    virtual void Update(float time);
    virtual void Render();
    
private:
    GameScreen* _GameScreen;
};
