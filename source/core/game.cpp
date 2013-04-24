#include "pixelboost/audio/audioManagerSimple.h"
#include "pixelboost/db/database.h"
#include "pixelboost/db/register.h"
#include "pixelboost/graphics/renderer/common/renderer.h"
#include "pixelboost/graphics/renderer/model/modelRenderer.h"
#include "pixelboost/graphics/renderer/sprite/spriteRenderer.h"
#include "pixelboost/graphics/renderer/text/textRenderer.h"
#include "pixelboost/graphics/resources/modelResource.h"
#include "pixelboost/graphics/resources/shaderResource.h"
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
    
    pb::ResourceManager::Instance()->GetPool("default")->CacheResource<pb::ShaderResource>("/shaders/texturedLit.shc");
    
    pb::TextRenderer::Instance()->LoadFont("font", "/fonts/font");
    
    pb::ResourceManager::Instance()->GetPool("default")->CacheResource<pb::ModelResource>("/models/cog.mdl");
    pb::ResourceManager::Instance()->GetPool("default")->CacheResource<pb::ModelResource>("/models/skybox.mdl");
    pb::ResourceManager::Instance()->GetPool("default")->CacheResource<pb::ModelResource>("/models/homingMine.mdl");
    pb::ResourceManager::Instance()->GetPool("default")->CacheResource<pb::ModelResource>("/models/staticMine.mdl");
    pb::ResourceManager::Instance()->GetPool("default")->CacheResource<pb::ModelResource>("/models/stealthBomber.mdl");
    pb::ResourceManager::Instance()->GetPool("default")->CacheResource<pb::ModelResource>("/models/turret.mdl");
    pb::ResourceManager::Instance()->GetPool("default")->CacheResource<pb::ModelResource>("/models/turret_homing.mdl");
    pb::ResourceManager::Instance()->GetPool("default")->CacheResource<pb::ModelResource>("/models/turret_laser.mdl");
    pb::ResourceManager::Instance()->GetPool("default")->CacheResource<pb::ModelResource>("/models/ship_01.mdl");
    pb::ResourceManager::Instance()->GetPool("default")->CacheResource<pb::ModelResource>("/models/ship_02.mdl");
    pb::ResourceManager::Instance()->GetPool("default")->CacheResource<pb::ModelResource>("/models/shield.mdl");
    pb::ResourceManager::Instance()->GetPool("default")->CacheResource<pb::ModelResource>("/models/asteroid01.mdl");
    pb::ResourceManager::Instance()->GetPool("default")->CacheResource<pb::ModelResource>("/models/asteroid02.mdl");
    pb::ResourceManager::Instance()->GetPool("default")->CacheResource<pb::ModelResource>("/models/asteroid03.mdl");
    pb::ResourceManager::Instance()->GetPool("default")->CacheResource<pb::ModelResource>("/models/asteroid04.mdl");
    pb::ResourceManager::Instance()->GetPool("default")->CacheResource<pb::ModelResource>("/models/weapon_laser.mdl");
    
    /*
     pb::ModelRenderer::Instance()->LoadModel("cog", "/models/cog.mdl");
     pb::ModelRenderer::Instance()->LoadModel("skybox", "/models/skybox.mdl");
     pb::ModelRenderer::Instance()->LoadModel("homingMine", "/models/homingMine.mdl");
     pb::ModelRenderer::Instance()->LoadModel("staticMine", "/models/staticMine.mdl");
     pb::ModelRenderer::Instance()->LoadModel("stealthBomber", "/models/stealthBomber.mdl");
     pb::ModelRenderer::Instance()->LoadModel("turret", "/models/turret.mdl");
     pb::ModelRenderer::Instance()->LoadModel("turret_homing", "/models/turret_homing.mdl");
     pb::ModelRenderer::Instance()->LoadModel("turret_laser", "/models/turret_laser.mdl");
     pb::ModelRenderer::Instance()->LoadModel("ship_01", "/models/ship_01.mdl");
     pb::ModelRenderer::Instance()->LoadModel("ship_02", "/models/ship_02.mdl");
     pb::ModelRenderer::Instance()->LoadModel("shield", "/models/shield.mdl");
     pb::ModelRenderer::Instance()->LoadModel("asteroid_01", "/models/asteroid01.mdl");
     pb::ModelRenderer::Instance()->LoadModel("asteroid_02", "/models/asteroid02.mdl");
     pb::ModelRenderer::Instance()->LoadModel("asteroid_03", "/models/asteroid03.mdl");
     pb::ModelRenderer::Instance()->LoadModel("asteroid_04", "/models/asteroid04.mdl");
     pb::ModelRenderer::Instance()->LoadModel("weapon_laser", "/models/weapon_laser.mdl");
     */
    
    pb::ModelRenderer::Instance()->LoadTexture("star", "/models/star.png");
    pb::ModelRenderer::Instance()->LoadTexture("asteroid", "/models/asteroid.png");
    pb::ModelRenderer::Instance()->LoadTexture("skybox", "/models/skybox.png");
    pb::ModelRenderer::Instance()->LoadTexture("cog_DIFF", "/models/cog_DIFF.png");
    pb::ModelRenderer::Instance()->LoadTexture("ship_01_DIFF", "/models/ship_01_DIFF.png");
    pb::ModelRenderer::Instance()->LoadTexture("ship_02_DIFF", "/models/ship_02_DIFF.png");
    pb::ModelRenderer::Instance()->LoadTexture("shield_DIFF", "/models/shield_DIFF.png");
    pb::ModelRenderer::Instance()->LoadTexture("staticMine_DIFF", "/models/staticMine_DIFF.png");
    pb::ModelRenderer::Instance()->LoadTexture("staticMine_armed_DIFF", "/models/staticMine_armed_DIFF.png");
    pb::ModelRenderer::Instance()->LoadTexture("stealthBomber_DIFF", "/models/stealthBomber_DIFF.png");
    pb::ModelRenderer::Instance()->LoadTexture("homingMine_active_DIFF", "/models/homingMine_active_DIFF.png");
    pb::ModelRenderer::Instance()->LoadTexture("homingMine_armed_DIFF", "/models/homingMine_armed_DIFF.png");
    
    pb::ModelRenderer::Instance()->LoadTexture("weapon_laser", "/models/weapon_laser.png");
    pb::ModelRenderer::Instance()->LoadTexture("grey", "/models/grey.png");
    pb::ModelRenderer::Instance()->LoadTexture("turret_weapons", "/models/turret_weapons.png");
    pb::ModelRenderer::Instance()->LoadTexture("turret_homing", "/models/turret_homing.png");
    pb::ModelRenderer::Instance()->LoadTexture("turret_laser", "/models/turret_laser.png");
    
    // TODO: Handle shader loading
    //    pb::Engine::Instance()->GetSpriteRenderer()->LoadSpriteSheet("game", "jpa");
    //    pb::Renderer::Instance()->GetShaderManager()->LoadShader("/shaders/texturedLit.shc");
    
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
    return pb::ResourceManager::Instance()->GetPool("default")->GetResource<pb::ShaderResource>("/shaders/texturedLit.shc")->GetShader();
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
