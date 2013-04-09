#pragma once

#include "pixelboost/framework/engine.h"

namespace pb
{
    class Shader;
}

class GameScreen;
class MenuScreen;

enum GameMode
{
	kGameModeGame,
	kGameModeMenu,
};

class Game : public pb::Engine
{
public:
    Game(void* platformContext, int argc, const char** argv);
    ~Game();
    
    static Game* Instance();
    
    virtual void Update(float timeDelta, float gameDelta);
    virtual void Render();
    
    void SetMode(GameMode mode);
    
    GameScreen* GetGameScreen();
    MenuScreen* GetMenuScreen();
    
    pb::Shader* GetLitShader();
    
private:
    GameScreen* _GameScreen;
    MenuScreen* _MenuScreen;
    
    GameMode _Mode;
};
