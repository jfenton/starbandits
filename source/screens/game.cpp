#include "pixelboost/framework/engine.h"
#include "pixelboost/graphics/camera/camera.h"
#include "pixelboost/graphics/camera/viewport.h"
#include "pixelboost/graphics/device/device.h"
#include "pixelboost/graphics/renderer/common/renderer.h"
#include "pixelboost/graphics/renderer/model/modelRenderer.h"
#include "pixelboost/graphics/renderer/sprite/spriteRenderer.h"
#include "pixelboost/graphics/shader/manager.h"
#include "pixelboost/logic/component/transform.h"
#include "pixelboost/logic/system/debug/render.h"
#include "pixelboost/logic/system/graphics/render/bounds.h"
#include "pixelboost/logic/system/physics/2d/physics.h"
#include "pixelboost/logic/scene.h"

#include "background/background.h"
#include "background/planet.h"
#include "common/layers.h"
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
    
    _Camera->Position.y = pb::Engine::Instance()->GetGameTime() * 4.f;
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
}

void GameScreen::SetActive(bool active)
{
    Screen::SetActive(active);
    
    if (active)
    {
        pb::Engine::Instance()->GetFontRenderer()->LoadFont(pb::kFileLocationBundle, "font", "/data/fonts/font");
        
        pb::Engine::Instance()->GetModelRenderer()->LoadModel(pb::kFileLocationBundle, "skybox", "/data/models/skybox.mdl");
        pb::Engine::Instance()->GetModelRenderer()->LoadModel(pb::kFileLocationBundle, "homingMine", "/data/models/homingMine.mdl");
        pb::Engine::Instance()->GetModelRenderer()->LoadModel(pb::kFileLocationBundle, "staticMine", "/data/models/staticMine.mdl");
        pb::Engine::Instance()->GetModelRenderer()->LoadModel(pb::kFileLocationBundle, "stealthBomber", "/data/models/stealthBomber.mdl");
        pb::Engine::Instance()->GetModelRenderer()->LoadModel(pb::kFileLocationBundle, "turret", "/data/models/turret.mdl");
        pb::Engine::Instance()->GetModelRenderer()->LoadModel(pb::kFileLocationBundle, "ship", "/data/models/ship.mdl");
        pb::Engine::Instance()->GetModelRenderer()->LoadModel(pb::kFileLocationBundle, "asteroid_01", "/data/models/asteroid01.mdl");
        pb::Engine::Instance()->GetModelRenderer()->LoadModel(pb::kFileLocationBundle, "asteroid_02", "/data/models/asteroid02.mdl");
        pb::Engine::Instance()->GetModelRenderer()->LoadModel(pb::kFileLocationBundle, "asteroid_03", "/data/models/asteroid03.mdl");
        pb::Engine::Instance()->GetModelRenderer()->LoadModel(pb::kFileLocationBundle, "asteroid_04", "/data/models/asteroid04.mdl");
        pb::Engine::Instance()->GetModelRenderer()->LoadModel(pb::kFileLocationBundle, "weapon_laser", "/data/models/weapon_laser.mdl");

        pb::Engine::Instance()->GetModelRenderer()->LoadTexture(pb::kFileLocationBundle, "asteroid", "/data/models/asteroid.png");
        pb::Engine::Instance()->GetModelRenderer()->LoadTexture(pb::kFileLocationBundle, "skybox", "/data/models/skybox.png");
        pb::Engine::Instance()->GetModelRenderer()->LoadTexture(pb::kFileLocationBundle, "ship_DIFF", "/data/models/ship_DIFF.png");
        pb::Engine::Instance()->GetModelRenderer()->LoadTexture(pb::kFileLocationBundle, "staticMine_DIFF", "/data/models/staticMine_DIFF.png");
        pb::Engine::Instance()->GetModelRenderer()->LoadTexture(pb::kFileLocationBundle, "staticMine_armed_DIFF", "/data/models/staticMine_armed_DIFF.png");
        pb::Engine::Instance()->GetModelRenderer()->LoadTexture(pb::kFileLocationBundle, "stealthBomber_DIFF", "/data/models/stealthBomber_DIFF.png");
        pb::Engine::Instance()->GetModelRenderer()->LoadTexture(pb::kFileLocationBundle, "grey", "/data/models/grey.png");
        
        pb::Engine::Instance()->GetSpriteRenderer()->LoadSpriteSheet(pb::kFileLocationBundle, "game", "jpa");
        
        pb::Renderer::Instance()->GetShaderManager()->LoadShader("/data/shaders/texturedLit.shc");
        
        _Camera = new pb::PerspectiveCamera();
        _Camera->FieldOfView = 45.f;
        _Camera->Position.z = 50.f;
        _Camera->ZNear = 40.f;
        _Camera->ZFar = 2000.f;
        
        _Scene = new pb::Scene();
        _Scene->AddSystem(new pb::BoundsRenderSystem());
        _Scene->AddSystem(new pb::PhysicsSystem2D(glm::vec2(0,0)));
        _Scene->AddSystem(new pb::DebugRenderSystem());
        
        _Player = new PlayerShip(_Scene, 0);
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
        
        delete _Camera;
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
