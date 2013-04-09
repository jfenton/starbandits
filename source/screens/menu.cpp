#include "pixelboost/framework/engine.h"
#include "pixelboost/graphics/camera/viewport.h"
#include "pixelboost/graphics/device/device.h"
#include "pixelboost/graphics/renderer/font/fontRenderer.h"
#include "pixelboost/logic/system/graphics/render/bounds.h"
#include "pixelboost/logic/scene.h"

#include "core/game.h"
#include "menu/menuItem.h"
#include "screens/menu.h"

MenuScreen::MenuScreen()
{
    
}

MenuScreen::~MenuScreen()
{
    
}

void MenuScreen::Update(float timeDelta, float gameDelta)
{
    _Scene->Update(timeDelta, gameDelta);
}

void MenuScreen::SetActive(bool active)
{
    Screen::SetActive(active);
    
    if (active)
    {
        _Scene = new pb::Scene();
        _Scene->AddSystem(new pb::BoundsRenderSystem());
        
        _Scene->CreateEntity<MenuItem>(0, 0);
        
        pb::GraphicsDevice::Instance()->SetClearColor(glm::vec4(0.2,0.2,0.2,1));
        
        _Viewport = new pb::Viewport(0, 0);
        _Viewport->SetScene(_Scene);
        
        AddViewport(_Viewport);
    } else {
        DestroyViewport(_Viewport);
        _Viewport = 0;
        
        delete _Scene;
    }
}
