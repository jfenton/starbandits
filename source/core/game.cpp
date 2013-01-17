#include "pixelboost/db/database.h"
#include "pixelboost/graphics/renderer/common/renderer.h"
#include "pixelboost/graphics/shader/manager.h"

#include "core/game.h"
#include "database/entities/register.h"
#include "database/records/register.h"
#include "screens/game.h"

namespace pb
{
    Engine* Engine::Create(void* platformContext, std::vector<std::string> args)
    {
        return new Game(platformContext);
    }
}

Game::Game(void* viewController)
    : pb::Engine(viewController)
{
    RegisterEntityCreation();
    RegisterRecordCreation();
    
    pb::Database::Instance()->SetLocation("/data/gamedata/");
    pb::Database::Instance()->OpenDatabase();

    _GameScreen = new GameScreen();
    
    _GameScreen->SetActive(true);
}

Game::~Game()
{
    delete _GameScreen;
}

Game* Game::Instance()
{
    return static_cast<Game*>(pb::Engine::Instance());
}

void Game::Update(float time)
{
    Engine::Update(time);
    
    _GameScreen->Update(time);
}

void Game::Render()
{
    Engine::Render();
}

GameScreen* Game::GetGameScreen()
{
    return _GameScreen;
}

pb::Shader* Game::GetLitShader()
{
    return pb::Renderer::Instance()->GetShaderManager()->GetShader("/data/shaders/texturedLit.shc");
}
