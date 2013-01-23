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
#include "gameplay/health.h"
#include "player/player.h"
#include "ui/ui.h"

GameUi::GameUi(pb::Scene* scene, pb::Uid playerId, int index)
    : pb::Entity(scene, 0)
    , _PlayerId(playerId)
{
    pb::BasicTransformComponent* transform = new pb::BasicTransformComponent(this);
    transform->SetPosition(glm::vec3(index == 0 ? -620.f/32.f : 620.f/32.f, 360.f/32.f - 1.f, 0));
    
    pb::FontComponent* text = new pb::FontComponent(this, "font", "");
    text->SetAlignment(index == 0 ? pb::kFontAlignLeft : pb::kFontAlignRight);
    text->SetRenderPass(pb::kRenderPassUi);
    text->SetLayer(kGraphicLayerUi);
    text->SetSize(0.8f);
    
    RegisterMessageHandler<pb::UpdateMessage>(MessageHandler(this, &GameUi::OnUpdate));
}

GameUi::~GameUi()
{
    UnregisterMessageHandler<pb::UpdateMessage>(MessageHandler(this, &GameUi::OnUpdate));
}

pb::Uid GameUi::GetType() const
{
    return GetStaticType();
}

pb::Uid GameUi::GetStaticType()
{
    return pb::TypeHash("GameUi");
}

void GameUi::OnUpdate(const pb::Message& message)
{
    const pb::UpdateMessage& updateMessage = static_cast<const pb::UpdateMessage&>(message);
    
    PlayerShip* ship = static_cast<PlayerShip*>(GetScene()->GetEntityById(_PlayerId));
    
    if (!ship)
    {
        GetComponentByType<pb::FontComponent>()->SetText("No Life Remaining");
        return;
    }
    
    float energy = ship->GetEnergy();
    HealthComponent* health = ship->GetComponentByType<HealthComponent>();

    if (energy && health)
    {
        char text[128];
        snprintf(text, 128, "Health: %.0f\nShields: %.0f\nEnergy: %.0f", health->GetHealth(), health->GetShields(), energy);
        GetComponentByType<pb::FontComponent>()->SetText(text);
    }
}
