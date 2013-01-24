#include "Box2D/Box2D.h"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/random.hpp"

#include "pixelboost/framework/engine.h"
#include "pixelboost/logic/component/graphics/font.h"
#include "pixelboost/logic/component/graphics/sprite.h"
#include "pixelboost/logic/component/transform/basic.h"
#include "pixelboost/logic/message/update.h"
#include "pixelboost/logic/system/graphics/render/render.h"
#include "pixelboost/logic/scene.h"

#include "common/layers.h"
#include "core/game.h"
#include "gameplay/health.h"
#include "menu/menuItem.h"
#include "player/player.h"
#include "screens/game.h"

MenuItem::MenuItem(pb::Scene* scene)
    : pb::Entity(scene, 0)
{
    _ShowingControls = false;
    
    pb::BasicTransformComponent* transform = new pb::BasicTransformComponent(this);
    transform->SetPosition(glm::vec3(0.f, 0.f, 0.f));
    
    pb::SpriteComponent* sprite = new pb::SpriteComponent(this, "menu");
    sprite->SetRenderPass(pb::kRenderPassUi);
    
    pb::FontComponent* single = new pb::FontComponent(this, "font", "Press O for One Player");
    single->SetAlignment(pb::kFontAlignLeft);
    single->SetRenderPass(pb::kRenderPassUi);
    single->SetLayer(kGraphicLayerUi);
    single->SetSize(0.5f);
    single->SetLocalTransform(glm::translate(glm::mat4x4(), glm::vec3(-620.f/32.f, 340.f/32.f, 0.f)));
    
    pb::FontComponent* two = new pb::FontComponent(this, "font", "Press A for Two Player");
    two->SetAlignment(pb::kFontAlignRight);
    two->SetRenderPass(pb::kRenderPassUi);
    two->SetLayer(kGraphicLayerUi);
    two->SetSize(0.5f);
    two->SetLocalTransform(glm::translate(glm::mat4x4(), glm::vec3(620.f/32.f, 340.f/32.f, 0.f)));
    
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
    Entity::ComponentList text = GetComponentsByType<pb::FontComponent>();
    
    for (Entity::ComponentList::iterator it = text.begin(); it != text.end(); ++it)
    {
        bool alpha = glm::mod(Game::Instance()->GetGameTime(), 1.5f) < 0.75f;
        static_cast<pb::FontComponent*>(*it)->SetTint(glm::vec4(1,1,1,alpha?1.f:0.f));
    }
}

bool MenuItem::OnButtonDown(int joystick, int button)
{
    if (_ShowingControls)
    {
        Game::Instance()->SetMode(kGameModeGame);
    } else {
        if (button == 0) {
            Game::Instance()->GetGameScreen()->SetNumPlayers(1);
            GetComponentByType<pb::SpriteComponent>()->SetSprite("control");
            _ShowingControls = true;
        } else if (button == 1) {
            Game::Instance()->GetGameScreen()->SetNumPlayers(2);
            GetComponentByType<pb::SpriteComponent>()->SetSprite("control");
            _ShowingControls = true;
        }
        
        if (_ShowingControls)
        {
            Entity::ComponentList text = GetComponentsByType<pb::FontComponent>();
            
            for (Entity::ComponentList::iterator it = text.begin(); it != text.end(); ++it)
            {
                DestroyComponent(*it);
            }
            
            pb::FontComponent* single = new pb::FontComponent(this, "font", "Press O to Start");
            single->SetAlignment(pb::kFontAlignCenter);
            single->SetRenderPass(pb::kRenderPassUi);
            single->SetLayer(kGraphicLayerUi);
            single->SetSize(0.5f);
            single->SetTint(glm::vec4(0,0,0,1));
            single->SetLocalTransform(glm::translate(glm::mat4x4(), glm::vec3(0.f, -340.f/32.f, 0.f)));
        }
    }
    
    return false;
}

bool MenuItem::OnButtonUp(int joystick, int button)
{
    return false;
}
