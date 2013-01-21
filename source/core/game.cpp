#include "pixelboost/db/database.h"
#include "pixelboost/graphics/renderer/common/renderer.h"
#include "pixelboost/graphics/renderer/font/fontRenderer.h"
#include "pixelboost/graphics/renderer/model/modelRenderer.h"
#include "pixelboost/graphics/renderer/sprite/spriteRenderer.h"
#include "pixelboost/graphics/shader/manager.h"

#include "core/game.h"
#include "database/entities/register.h"
#include "database/records/register.h"
#include "screens/game.h"
#include "screens/menu.h"

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
    _MenuScreen = new MenuScreen();
    
    pb::Engine::Instance()->GetFontRenderer()->LoadFont(pb::kFileLocationBundle, "font", "/data/fonts/font");
    
    pb::Engine::Instance()->GetModelRenderer()->LoadModel(pb::kFileLocationBundle, "skybox", "/data/models/skybox.mdl");
    pb::Engine::Instance()->GetModelRenderer()->LoadModel(pb::kFileLocationBundle, "homingMine", "/data/models/homingMine.mdl");
    pb::Engine::Instance()->GetModelRenderer()->LoadModel(pb::kFileLocationBundle, "staticMine", "/data/models/staticMine.mdl");
    pb::Engine::Instance()->GetModelRenderer()->LoadModel(pb::kFileLocationBundle, "stealthBomber", "/data/models/stealthBomber.mdl");
    pb::Engine::Instance()->GetModelRenderer()->LoadModel(pb::kFileLocationBundle, "turret", "/data/models/turret.mdl");
    pb::Engine::Instance()->GetModelRenderer()->LoadModel(pb::kFileLocationBundle, "ship_01", "/data/models/ship_01.mdl");
    pb::Engine::Instance()->GetModelRenderer()->LoadModel(pb::kFileLocationBundle, "ship_02", "/data/models/ship_02.mdl");
    pb::Engine::Instance()->GetModelRenderer()->LoadModel(pb::kFileLocationBundle, "shield", "/data/models/shield.mdl");
    pb::Engine::Instance()->GetModelRenderer()->LoadModel(pb::kFileLocationBundle, "asteroid_01", "/data/models/asteroid01.mdl");
    pb::Engine::Instance()->GetModelRenderer()->LoadModel(pb::kFileLocationBundle, "asteroid_02", "/data/models/asteroid02.mdl");
    pb::Engine::Instance()->GetModelRenderer()->LoadModel(pb::kFileLocationBundle, "asteroid_03", "/data/models/asteroid03.mdl");
    pb::Engine::Instance()->GetModelRenderer()->LoadModel(pb::kFileLocationBundle, "asteroid_04", "/data/models/asteroid04.mdl");
    pb::Engine::Instance()->GetModelRenderer()->LoadModel(pb::kFileLocationBundle, "weapon_laser", "/data/models/weapon_laser.mdl");
    
    pb::Engine::Instance()->GetModelRenderer()->LoadTexture(pb::kFileLocationBundle, "star", "/data/models/star.png");
    pb::Engine::Instance()->GetModelRenderer()->LoadTexture(pb::kFileLocationBundle, "asteroid", "/data/models/asteroid.png");
    pb::Engine::Instance()->GetModelRenderer()->LoadTexture(pb::kFileLocationBundle, "skybox", "/data/models/skybox.png");
    pb::Engine::Instance()->GetModelRenderer()->LoadTexture(pb::kFileLocationBundle, "ship_01_DIFF", "/data/models/ship_01_DIFF.png");
    pb::Engine::Instance()->GetModelRenderer()->LoadTexture(pb::kFileLocationBundle, "ship_02_DIFF", "/data/models/ship_02_DIFF.png");
    pb::Engine::Instance()->GetModelRenderer()->LoadTexture(pb::kFileLocationBundle, "shield_DIFF", "/data/models/shield_DIFF.png");
    pb::Engine::Instance()->GetModelRenderer()->LoadTexture(pb::kFileLocationBundle, "staticMine_DIFF", "/data/models/staticMine_DIFF.png");
    pb::Engine::Instance()->GetModelRenderer()->LoadTexture(pb::kFileLocationBundle, "staticMine_armed_DIFF", "/data/models/staticMine_armed_DIFF.png");
    pb::Engine::Instance()->GetModelRenderer()->LoadTexture(pb::kFileLocationBundle, "stealthBomber_DIFF", "/data/models/stealthBomber_DIFF.png");
    pb::Engine::Instance()->GetModelRenderer()->LoadTexture(pb::kFileLocationBundle, "grey", "/data/models/grey.png");
    
    pb::Engine::Instance()->GetSpriteRenderer()->LoadSpriteSheet(pb::kFileLocationBundle, "game", "jpa");
    
    pb::Renderer::Instance()->GetShaderManager()->LoadShader("/data/shaders/texturedLit.shc");
    
    _Mode = kGameModeMenu;
    _MenuScreen->SetActive(true);

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
    
    switch (_Mode)
    {
        case kGameModeGame:
            _GameScreen->Update(time);
            break;

        case kGameModeMenu:
            _MenuScreen->Update(time);
            break;
    }
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

void Game::SetMode(GameMode mode)
{
    switch (_Mode)
    {
        case kGameModeGame:
            _GameScreen->SetActive(false);
            break;
        case kGameModeMenu:
            _MenuScreen->SetActive(false);
            break;
    }
    
    _Mode = mode;

    switch (_Mode)
    {
        case kGameModeGame:
            _GameScreen->SetActive(true);
            break;
        case kGameModeMenu:
            _MenuScreen->SetActive(true);
            break;
    }
}
