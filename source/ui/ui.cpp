#include "Box2D/Box2D.h"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/random.hpp"

#include "pixelboost/framework/engine.h"
#include "pixelboost/graphics/renderer/sprite/spriteRenderer.h"
#include "pixelboost/logic/component/graphics/font.h"
#include "pixelboost/logic/component/transform.h"
#include "pixelboost/logic/message/update.h"
#include "pixelboost/logic/system/graphics/render/render.h"
#include "pixelboost/logic/scene.h"

#include "common/layers.h"
#include "core/game.h"
#include "gameplay/health.h"
#include "player/player.h"
#include "screens/game.h"
#include "ui/ui.h"

PB_DEFINE_ENTITY(GameUi)

GameUi::GameUi(pb::Scene* scene, pb::Entity* parent, pb::DbEntity* creationEntity)
    : pb::Entity(scene, parent, creationEntity)
    , _Score(0)
    , _PlayerId(0)
{
}

void GameUi::Initialise(pb::Uid playerId, int index)
{
    _PlayerId = playerId;

    CreateComponent<pb::TransformComponent>();
    
    pb::FontComponent* text = CreateComponent<pb::FontComponent>();
    text->SetFont("font");
    text->SetAlignment(index == 0 ? pb::kFontAlignLeft : pb::kFontAlignRight);
    text->SetRenderPass(pb::kRenderPassUi);
    text->SetLayer(kGraphicLayerUi);
    text->SetSize(0.8f);
    text->SetLocalTransform(glm::translate(glm::mat4x4(), glm::vec3(index == 0 ? -620.f/32.f : 620.f/32.f, 360.f/32.f - 1.f, 0)));
    
    /*
    if (index == 0)
    {
        _Score = new pb::FontComponent(this, "font", "00000000");
        _Score->SetAlignment(pb::kFontAlignCenter);
        _Score->SetRenderPass(pb::kRenderPassUi);
        _Score->SetLayer(kGraphicLayerUi);
        _Score->SetSize(0.8f);
        _Score->SetLocalTransform(glm::translate(glm::mat4x4(), glm::vec3(0.f, 360.f/32.f - 1.f, 0)));
    }
    */
    
    RegisterMessageHandler<pb::UpdateMessage>(pb::MessageHandler(this, &GameUi::OnUpdate));
}

GameUi::~GameUi()
{
    UnregisterMessageHandler<pb::UpdateMessage>(pb::MessageHandler(this, &GameUi::OnUpdate));
}

void GameUi::OnUpdate(const pb::Message& message)
{
    const pb::UpdateMessage& updateMessage = static_cast<const pb::UpdateMessage&>(message);
    
    if (_Score)
    {
        char text[128];
        sprintf(text, "%08.f", Game::Instance()->GetGameScreen()->GetScore());
        _Score->SetText(text);
    }
    
    PlayerShip* ship = static_cast<PlayerShip*>(GetScene()->GetEntityById(_PlayerId));
    
    if (!ship)
    {
        GetComponent<pb::FontComponent>()->SetText("No Life Remaining");
        return;
    }
    
    float energy = ship->GetEnergy();
    HealthComponent* health = ship->GetComponent<HealthComponent>();

    if (energy && health)
    {
        char text[128];
        snprintf(text, 128, "Health: %.0f\nShields: %.0f\nEnergy: %.0f", health->GetHealth(), health->GetShields(), energy);
        GetComponent<pb::FontComponent>()->SetText(text);
    }
}
