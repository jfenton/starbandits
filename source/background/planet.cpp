#include "Box2D/Box2D.h"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/random.hpp"

#include "pixelboost/framework/engine.h"
#include "pixelboost/graphics/renderer/sprite/spriteRenderer.h"
#include "pixelboost/logic/component/graphics/sprite.h"
#include "pixelboost/logic/component/transform.h"
#include "pixelboost/logic/message/update.h"
#include "pixelboost/logic/system/graphics/render/render.h"
#include "pixelboost/logic/scene.h"

#include "background/planet.h"
#include "common/layers.h"

PB_DEFINE_ENTITY(Planet)

Planet::Planet(pb::Scene* scene, pb::Entity* parent, pb::DbEntity* creationEntity)
    : pb::Entity(scene, parent, creationEntity)
{

}

Planet::~Planet()
{
    UnregisterMessageHandler<pb::UpdateMessage>(pb::MessageHandler(this, &Planet::OnUpdate));
}

void Planet::Initialise(const std::string& name, glm::vec3 position, float size)
{
    auto transform = CreateComponent<pb::TransformComponent>();
    transform->SetPosition(position);
    transform->SetScale(glm::vec3(size, size, size));
    
    auto sprite = CreateComponent<pb::SpriteComponent>();
    sprite->SetSprite("/spritesheets/game", name);
    sprite->SetLayer(kGraphicLayerPlanets);
    
    RegisterMessageHandler<pb::UpdateMessage>(pb::MessageHandler(this, &Planet::OnUpdate));
}

void Planet::OnUpdate(const pb::Message& message)
{
    const pb::UpdateMessage& updateMessage = static_cast<const pb::UpdateMessage&>(message);
    
    glm::vec3 position = GetComponent<pb::TransformComponent>()->GetPosition();
    GetComponent<pb::TransformComponent>()->SetPosition(position + glm::vec3(-0.03, 0, 0.02));
}
