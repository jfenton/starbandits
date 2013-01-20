#include "Box2D/Box2D.h"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/random.hpp"

#include "pixelboost/framework/engine.h"
#include "pixelboost/graphics/renderer/sprite/spriteRenderer.h"
#include "pixelboost/logic/component/graphics/font.h"
#include "pixelboost/logic/component/transform/basic.h"
#include "pixelboost/logic/message/update.h"
#include "pixelboost/logic/system/graphics/render/render.h"
#include "pixelboost/logic/scene.h"

#include "common/layers.h"
#include "core/game.h"
#include "gameplay/health.h"
#include "menu/menuItem.h"
#include "player/player.h"

MenuItem::MenuItem(pb::Scene* scene)
    : pb::Entity(scene, 0)
{
    pb::BasicTransformComponent* transform = new pb::BasicTransformComponent(this);
    transform->SetPosition(glm::vec3(0.f, 0.f, 0.f));
    
    pb::FontComponent* text = new pb::FontComponent(this, "font", "Press O for One Player\n\nPress A For Two Player");
    text->SetAlignment(pb::kFontAlignCenter);
    text->SetRenderPass(pb::kRenderPassUi);
    text->SetLayer(kGraphicLayerUi);
    
    RegisterMessageHandler<pb::UpdateMessage>(MessageHandler(this, &MenuItem::OnUpdate));
    
    pb::Engine::Instance()->GetJoystickManager()->AddHandler(this);
}

MenuItem::~MenuItem()
{
    pb::Engine::Instance()->GetJoystickManager()->RemoveHandler(this);
    
    UnregisterMessageHandler<pb::UpdateMessage>(MessageHandler(this, &MenuItem::OnUpdate));
}

pb::Uid MenuItem::GetType() const
{
    return GetStaticType();
}

pb::Uid MenuItem::GetStaticType()
{
    return pb::TypeHash("MenuItem");
}

void MenuItem::OnUpdate(const pb::Message& message)
{
    
}

bool MenuItem::OnButtonDown(int joystick, int button)
{
    if (button == 0) {
        Game::Instance()->SetMode(kGameModeGame);
    } else if (button == 1) {
        Game::Instance()->SetMode(kGameModeGame);
    }
    
    return false;
}

bool MenuItem::OnButtonUp(int joystick, int button)
{
    return false;
}
