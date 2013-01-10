#include "pixelboost/framework/engine.h"
#include "pixelboost/graphics/camera/camera.h"
#include "pixelboost/graphics/camera/viewport.h"
#include "pixelboost/graphics/device/device.h"
#include "pixelboost/graphics/renderer/model/modelRenderer.h"
#include "pixelboost/graphics/renderer/sprite/spriteRenderer.h"
#include "pixelboost/logic/component/transform.h"
#include "pixelboost/logic/system/debug/render.h"
#include "pixelboost/logic/system/graphics/render/bounds.h"
#include "pixelboost/logic/system/physics/2d/physics.h"
#include "pixelboost/logic/scene.h"

#include "background/background.h"
#include "enemies/asteroid.h"
#include "player/ship.h"
#include "screens/game.h"

GameScreen::GameScreen()
    : _Camera(0)
    , _Scene(0)
    , _Viewport(0)
{
    
}

GameScreen::~GameScreen()
{
    
}

void GameScreen::Update(float time)
{
    _Scene->Update(time);
    
    _Camera->Position.y = pb::Engine::Instance()->GetGameTime() * 0.5f;
    
    pb::Scene::EntityMap tiles = _Scene->GetEntitiesByType<BackgroundTile>();
    
    float maxY = -1000000000;
    for (pb::Scene::EntityMap::iterator it = tiles.begin(); it != tiles.end(); ++it)
    {
        pb::Entity* tile = it->second;
        float tileY = tile->GetComponentByType<pb::TransformComponent>()->GetPosition().y;
        maxY = glm::max(maxY, tileY);
        
        if (tileY < _Camera->Position.y - 1024.f/32.f)
            tile->Destroy();
    }
    
    if (maxY > _Camera->Position.y - 1024.f/32.f)
    {
        new BackgroundTile(_Scene, glm::vec2(0, maxY + 1024.f/32.f));
    }
}

void GameScreen::SetActive(bool active)
{
    Screen::SetActive(active);
    
    if (active)
    {
        pb::Engine::Instance()->GetModelRenderer()->LoadModel(pb::kFileLocationBundle, "ship", "/data/models/ship.mdl");
        pb::Engine::Instance()->GetModelRenderer()->LoadTexture(pb::kFileLocationBundle, "ship", "/data/models/ship.png");
        pb::Engine::Instance()->GetModelRenderer()->LoadModel(pb::kFileLocationBundle, "asteroid_01", "/data/models/asteroid01.mdl");
        pb::Engine::Instance()->GetModelRenderer()->LoadModel(pb::kFileLocationBundle, "asteroid_02", "/data/models/asteroid02.mdl");
        pb::Engine::Instance()->GetModelRenderer()->LoadModel(pb::kFileLocationBundle, "asteroid_03", "/data/models/asteroid03.mdl");
        pb::Engine::Instance()->GetModelRenderer()->LoadModel(pb::kFileLocationBundle, "asteroid_04", "/data/models/asteroid04.mdl");
        pb::Engine::Instance()->GetModelRenderer()->LoadTexture(pb::kFileLocationBundle, "asteroid", "/data/models/asteroid.png");
        pb::Engine::Instance()->GetSpriteRenderer()->LoadSpriteSheet(pb::kFileLocationBundle, "game", "jpa");
        
        _Camera = new pb::PerspectiveCamera();
        _Camera->FieldOfView = 45.f;
        _Camera->Position.z = 50.f;
        _Camera->ZNear = 1.f;
        _Camera->ZFar = 10000.f;
        
        _Scene = new pb::Scene();
        _Scene->AddSystem(new pb::BoundsRenderSystem());
        _Scene->AddSystem(new pb::PhysicsSystem2D(glm::vec2(0,0)));
        _Scene->AddSystem(new pb::DebugRenderSystem());
        
        _Player = new PlayerShip(_Scene);
        new BackgroundTile(_Scene, glm::vec2(0,0));
        new Asteroid(_Scene, glm::vec2(((float)rand()/(float)RAND_MAX)*20.f, ((float)rand()/(float)RAND_MAX)*20.f));
        new Asteroid(_Scene, glm::vec2(((float)rand()/(float)RAND_MAX)*20.f, ((float)rand()/(float)RAND_MAX)*20.f));
        new Asteroid(_Scene, glm::vec2(((float)rand()/(float)RAND_MAX)*20.f, ((float)rand()/(float)RAND_MAX)*20.f));
        
        //_Scene->GetSystemByType<pb::PhysicsSystem2D>()->SetDebugRender(true);
        
        pb::GraphicsDevice::Instance()->SetClearColor(glm::vec4(0.2,0.2,0.2,1));
        
        _Viewport = new pb::Viewport(0, _Camera);
        _Viewport->SetScene(_Scene);

        AddViewport(_Viewport);
    } else {
        DestroyViewport(_Viewport);
        _Viewport = 0;
        
        delete _Camera;
    }
}
