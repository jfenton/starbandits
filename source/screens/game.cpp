#include "pixelboost/framework/engine.h"
#include "pixelboost/graphics/camera/camera.h"
#include "pixelboost/graphics/camera/viewport.h"
#include "pixelboost/graphics/device/device.h"
#include "pixelboost/graphics/renderer/sprite/spriteRenderer.h"
#include "pixelboost/logic/component/transform.h"
#include "pixelboost/logic/system/debug/render.h"
#include "pixelboost/logic/system/graphics/render/bounds.h"
#include "pixelboost/logic/system/physics/2d/physics.h"
#include "pixelboost/logic/scene.h"

#include "background/background.h"
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
    
    _Camera->Position.y = _Player->GetComponentByType<pb::TransformComponent>()->GetPosition().y;
    
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
        pb::Engine::Instance()->GetSpriteRenderer()->LoadSpriteSheet(pb::kFileLocationBundle, "game", "jpa");
        
        _Camera = new pb::OrthographicCamera();
        
        _Scene = new pb::Scene();
        _Scene->AddSystem(new pb::BoundsRenderSystem());
        _Scene->AddSystem(new pb::PhysicsSystem2D(glm::vec2(0,0)));
        _Scene->AddSystem(new pb::DebugRenderSystem());
        
        _Player = new PlayerShip(_Scene);
        new BackgroundTile(_Scene, glm::vec2(0,0));
        
        pb::GraphicsDevice::Instance()->SetClearColor(glm::vec4(0.5,0.5,0.5,1));
        
        _Viewport = new pb::Viewport(0, _Camera);
        _Viewport->SetScene(_Scene);

        AddViewport(_Viewport);
    } else {
        DestroyViewport(_Viewport);
        _Viewport = 0;
        
        delete _Camera;
    }
}
