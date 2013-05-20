#include "pixelboost/audio/audioManagerSimple.h"
#include "pixelboost/db/database.h"
#include "pixelboost/db/register.h"
#include "pixelboost/graphics/renderer/common/renderer.h"
#include "pixelboost/graphics/renderer/model/modelRenderer.h"
#include "pixelboost/graphics/renderer/sprite/spriteRenderer.h"
#include "pixelboost/graphics/renderer/text/textRenderer.h"
#include "pixelboost/graphics/resources/materialResource.h"
#include "pixelboost/graphics/resources/modelResource.h"
#include "pixelboost/graphics/resources/shaderResource.h"
#include "pixelboost/graphics/resources/spriteSheetResource.h"
#include "pixelboost/resource/resourceManager.h"

#include "core/game.h"
#include "screens/game.h"
#include "screens/menu.h"

PB_DB_DECLARE_NAMESPACE(GameEntities)
PB_DB_DECLARE_NAMESPACE(GameRecords)

namespace pb
{
    Engine* Engine::Create(void* platformContext, int argc, const char** argv)
    {
        return new Game(platformContext, argc, argv);
    }
}

Game::Game(void* platformContext, int argc, const char** argv)
    : pb::Engine(platformContext, argc, argv)
{
    
}

Game::~Game()
{
    delete _GameScreen;
}

Game* Game::Instance()
{
    return static_cast<Game*>(pb::Engine::Instance());
}

void Game::Initialise()
{
    pb::Engine::Initialise();
    
    PB_DB_REGISTER_NAMESPACE(pb::Database::Instance(), GameEntities)
    PB_DB_REGISTER_NAMESPACE(pb::Database::Instance(), GameRecords)
    
    pb::Database::Instance()->SetLocation("/database/");
    pb::Database::Instance()->OpenDatabase();
    
    _GameScreen = new GameScreen();
    _MenuScreen = new MenuScreen();
    
    pb::AudioManagerSimple::Instance()->LoadSfx("boom.wav");
    pb::AudioManagerSimple::Instance()->LoadSfx("boom_low.wav");
    pb::AudioManagerSimple::Instance()->LoadSfx("laser.wav");
    pb::AudioManagerSimple::Instance()->LoadSfx("missile.wav");
    
    pb::ResourceManager::Instance()->GetPool("default")->CacheResource<pb::ShaderResource>("/shaders/texturedLit");
    
    pb::TextRenderer::Instance()->LoadFont("font", "/fonts/font");
    
    pb::ResourceManager::Instance()->GetPool("default")->CacheResource<pb::ModelResource>("/models/cog");
    pb::ResourceManager::Instance()->GetPool("default")->CacheResource<pb::ModelResource>("/models/skybox");
    pb::ResourceManager::Instance()->GetPool("default")->CacheResource<pb::ModelResource>("/models/homingMine");
    pb::ResourceManager::Instance()->GetPool("default")->CacheResource<pb::ModelResource>("/models/staticMine");
    pb::ResourceManager::Instance()->GetPool("default")->CacheResource<pb::ModelResource>("/models/stealthBomber");
    pb::ResourceManager::Instance()->GetPool("default")->CacheResource<pb::ModelResource>("/models/turret");
    pb::ResourceManager::Instance()->GetPool("default")->CacheResource<pb::ModelResource>("/models/turret_homing");
    pb::ResourceManager::Instance()->GetPool("default")->CacheResource<pb::ModelResource>("/models/turret_laser");
    pb::ResourceManager::Instance()->GetPool("default")->CacheResource<pb::ModelResource>("/models/ship_01");
    pb::ResourceManager::Instance()->GetPool("default")->CacheResource<pb::ModelResource>("/models/ship_02");
    pb::ResourceManager::Instance()->GetPool("default")->CacheResource<pb::ModelResource>("/models/shield");
    pb::ResourceManager::Instance()->GetPool("default")->CacheResource<pb::ModelResource>("/models/asteroid01");
    pb::ResourceManager::Instance()->GetPool("default")->CacheResource<pb::ModelResource>("/models/asteroid02");
    pb::ResourceManager::Instance()->GetPool("default")->CacheResource<pb::ModelResource>("/models/asteroid03");
    pb::ResourceManager::Instance()->GetPool("default")->CacheResource<pb::ModelResource>("/models/asteroid04");
    pb::ResourceManager::Instance()->GetPool("default")->CacheResource<pb::ModelResource>("/models/weapon_laser");
    
    pb::ResourceManager::Instance()->GetPool("default")->CacheResource<pb::SpriteSheetResource>("/spritesheets/game");

    pb::ResourceManager::Instance()->GetPool("default")->CacheResource<pb::MaterialResource>("/materials/asteroid");
    pb::ResourceManager::Instance()->GetPool("default")->CacheResource<pb::MaterialResource>("/materials/staticMine");
    pb::ResourceManager::Instance()->GetPool("default")->CacheResource<pb::MaterialResource>("/materials/staticMine_armed");
    
    _Mode = kGameModeMenu;
    _MenuScreen->SetActive(true);
    
    pb::AudioManagerSimple::Instance()->PlayBgm("music.m4a", true);
}

void Game::Update(float timeDelta, float gameDelta)
{
    Engine::Update(timeDelta, gameDelta);
    
    switch (_Mode)
    {
        case kGameModeGame:
            _GameScreen->Update(timeDelta, gameDelta);
            break;

        case kGameModeMenu:
            _MenuScreen->Update(timeDelta, gameDelta);
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
    return pb::ResourceManager::Instance()->GetPool("default")->GetResource<pb::ShaderResource>("/shaders/texturedLit")->GetShader();
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
