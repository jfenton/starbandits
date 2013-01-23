#include "pixelboost/framework/engine.h"
#include "pixelboost/graphics/camera/camera.h"
#include "pixelboost/graphics/camera/viewport.h"
#include "pixelboost/graphics/device/device.h"
#include "pixelboost/graphics/shader/manager.h"
#include "pixelboost/logic/component/transform.h"
#include "pixelboost/logic/system/debug/render.h"
#include "pixelboost/logic/system/graphics/render/bounds.h"
#include "pixelboost/logic/system/physics/2d/physics.h"
#include "pixelboost/logic/scene.h"

#include "background/background.h"
#include "background/planet.h"
#include "background/stars.h"
#include "common/layers.h"
#include "core/game.h"
#include "enemies/asteroid.h"
#include "enemies/homingMine.h"
#include "enemies/staticMine.h"
#include "level/levelSegment.h"
#include "player/player.h"
#include "screens/game.h"
#include "ui/ui.h"

GameScreen::GameScreen()
    : _Camera(0)
    , _CurrentY(0.f)
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
    
    _Camera->Position.y += time * 4.f;
    _Background->GetComponentByType<pb::TransformComponent>()->SetPosition(_Camera->Position);
    
    pb::Scene::EntityMap tiles = _Scene->GetEntitiesByType<BackgroundTile>();
    
    if (_Camera->Position.y > _CurrentY - (720.f/32.f)*2.f)
    {
        if (_LevelSegment)
        {
            delete _LevelSegment;
        }
        
        _LevelSegment = new LevelSegment(_Scene, _CurrentY);
        
        _CurrentY = _CurrentY + _LevelSegment->GetLength();
    }
    
    if (_LevelSegment)
    {
        pb::Uid entityId = _LevelSegment->Create();
        
        if (!entityId)
        {
            delete _LevelSegment;
            _LevelSegment = 0;
        }
    }
    
    for (pb::Scene::EntityMap::const_iterator it = _Scene->GetEntities().begin(); it != _Scene->GetEntities().end(); ++it)
    {
        pb::Entity* entity = it->second;
        
        pb::Uid type = it->second->GetType();
        
        if (type == PlayerShip::GetStaticType() || type == Planet::GetStaticType() || type == BackgroundStars::GetStaticType() || type == GameUi::GetStaticType())
            continue;
        
        pb::TransformComponent* transform = entity->GetComponentByType<pb::TransformComponent>();
        
        if (!transform)
            continue;
        
        glm::vec3 position = transform->GetPosition();
        
        if (position.y < _Camera->Position.y - 1024.f/32.f || position.x < -GetArenaBounds()[2]-5.f || position.x > GetArenaBounds()[2]+5.f)
        {
            entity->Destroy();
        }
    }
    
    pb::Scene::EntityMap players = _Scene->GetEntitiesByType<PlayerShip>();
    
    if (players.size() == 0)
    {
        Game::Instance()->SetMode(kGameModeMenu);
    }
}

void GameScreen::SetActive(bool active)
{
    Screen::SetActive(active);
    
    if (active)
    {
        _LevelSegment = 0;
        _CurrentY = 50.f;
        _Camera = new pb::PerspectiveCamera();
        _Camera->FieldOfView = 45.f;
        _Camera->Position.z = 50.f;
        _Camera->ZNear = 40.f;
        _Camera->ZFar = 2000.f;
        
        _Scene = new pb::Scene();
        _Scene->AddSystem(new pb::BoundsRenderSystem());
        _Scene->AddSystem(new pb::PhysicsSystem2D(glm::vec2(0,0)));
        _Scene->AddSystem(new pb::DebugRenderSystem());
        
        for (int i=0; i<_NumPlayers; i++)
        {
            PlayerShip* ship = new PlayerShip(_Scene, i);
            new GameUi(_Scene, ship->GetUid(), i);
        }
        
        _Background = new BackgroundTile(_Scene, glm::vec2(0,0));
        new BackgroundStars(_Scene);
        
        new Planet(_Scene, "planet", glm::vec3(-36, 350, -500), 12.5);
        new Planet(_Scene, "moon", glm::vec3(-90, 150, -501), 12.5);
        
        //_Scene->GetSystemByType<pb::PhysicsSystem2D>()->SetDebugRender(true, kGraphicLayerPhysicsDebug);
        
        pb::GraphicsDevice::Instance()->SetClearColor(glm::vec4(0.2,0.2,0.2,1));
        
        _Viewport = new pb::Viewport(0, _Camera);
        _Viewport->SetScene(_Scene);

        AddViewport(_Viewport);
    } else {
        DestroyViewport(_Viewport);
        _Viewport = 0;
        
        if (_LevelSegment)
        {
            delete _LevelSegment;
            _LevelSegment = 0;
        }
        
        delete _Camera;
        delete _Scene;
    }
}

void GameScreen::SetNumPlayers(int numPlayers)
{
    _NumPlayers = numPlayers;
}

int GameScreen::GetNumPlayers()
{
    return _NumPlayers;
}

pb::PerspectiveCamera* GameScreen::GetCamera()
{
    return _Camera;
}

glm::vec4 GameScreen::GetArenaBounds()
{
    return glm::vec4(_Camera->Position.x, _Camera->Position.y, 31.25, 18.75);
}
