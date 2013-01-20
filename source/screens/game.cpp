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
    
    float currentSegmentLength = 0.f;
    
    if (_Segments.size())
    {
        currentSegmentLength = _Segments[_Segments.size()-1]->GetLength();
    }
    
    if (_Camera->Position.y > _CurrentY - (720.f/32.f)*2.f)
    {
        LevelSegment* levelSegment = new LevelSegment(_Scene, _CurrentY);
        levelSegment->Create();
        _Segments.push_back(levelSegment);
        
        _CurrentY = _CurrentY + levelSegment->GetLength();
        
        printf("Creating segment cam %f, current y %f, segment %f (%lu segments)\n", _Camera->Position.y, _CurrentY, levelSegment->GetLength(), _Segments.size());
    }
    
    for (std::vector<LevelSegment*>::iterator it = _Segments.begin(); it != _Segments.end();)
    {
        if (!(*it)->Cleanup(_Camera->Position.y - 1024.f/32.f, GetArenaBounds()))
        {
            delete *it;
            it = _Segments.erase(it);
        } else {
            ++it;
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
        _CurrentY = 0.f;
        _Camera = new pb::PerspectiveCamera();
        _Camera->FieldOfView = 45.f;
        _Camera->Position.z = 50.f;
        _Camera->ZNear = 40.f;
        _Camera->ZFar = 2000.f;
        
        _Scene = new pb::Scene();
        _Scene->AddSystem(new pb::BoundsRenderSystem());
        _Scene->AddSystem(new pb::PhysicsSystem2D(glm::vec2(0,0)));
        _Scene->AddSystem(new pb::DebugRenderSystem());
        
        new PlayerShip(_Scene, 0);
        _Background = new BackgroundTile(_Scene, glm::vec2(0,0));
        
        new Planet(_Scene, glm::vec3(-36, 350, -500), 12.5);
        
        new GameUi(_Scene);
        
        //_Scene->GetSystemByType<pb::PhysicsSystem2D>()->SetDebugRender(true, kGraphicLayerPhysicsDebug);
        
        pb::GraphicsDevice::Instance()->SetClearColor(glm::vec4(0.2,0.2,0.2,1));
        
        _Viewport = new pb::Viewport(0, _Camera);
        _Viewport->SetScene(_Scene);

        AddViewport(_Viewport);
    } else {
        DestroyViewport(_Viewport);
        _Viewport = 0;
        
        for (std::vector<LevelSegment*>::iterator it = _Segments.begin(); it != _Segments.end(); ++it)
        {
            delete *it;
        }
        _Segments.clear();

        delete _Camera;
        delete _Scene;
    }
}

pb::PerspectiveCamera* GameScreen::GetCamera()
{
    return _Camera;
}

glm::vec4 GameScreen::GetArenaBounds()
{
    return glm::vec4(_Camera->Position.x, _Camera->Position.y, 31.25, 18.75);
}
