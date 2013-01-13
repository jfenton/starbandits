#pragma once

#include "pixelboost/framework/engine.h"

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
    
private:
    GameScreen* _GameScreen;
};
