#include "Box2D/Box2D.h"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/random.hpp"

#include "pixelboost/framework/engine.h"
#include "pixelboost/graphics/renderer/sprite/spriteRenderer.h"
#include "pixelboost/logic/component/graphics/sprite.h"
#include "pixelboost/logic/component/transform/basic.h"
#include "pixelboost/logic/message/update.h"
#include "pixelboost/logic/system/graphics/render/render.h"
#include "pixelboost/logic/scene.h"

#include "background/planet.h"
#include "common/layers.h"

Planet::Planet(pb::Scene* scene, const std::string& name, glm::vec3 position, float size)
    : pb::Entity(scene, 0)
{
    pb::BasicTransformComponent* transform = new pb::BasicTransformComponent(this);
    transform->SetPosition(position);
    transform->SetScale(glm::vec3(size, size, size));
    
    pb::SpriteComponent* sprite = new pb::SpriteComponent(this, name);
    sprite->SetLayer(kGraphicLayerPlanets);
    
    RegisterMessageHandler<pb::UpdateMessage>(MessageHandler(this, &Planet::OnUpdate));
}

Planet::~Planet()
{
    UnregisterMessageHandler<pb::UpdateMessage>(MessageHandler(this, &Planet::OnUpdate));
}

pb::Uid Planet::GetType() const
{
    return GetStaticType();
}

pb::Uid Planet::GetStaticType()
{
    return pb::TypeHash("Planet");
}

void Planet::OnUpdate(const pb::Message& message)
{
    const pb::UpdateMessage& updateMessage = static_cast<const pb::UpdateMessage&>(message);
    
    glm::vec3 position = GetComponentByType<pb::TransformComponent>()->GetPosition();
    GetComponentByType<pb::TransformComponent>()->SetPosition(position + glm::vec3(-0.03, 0, 0.02));
}
