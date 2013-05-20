#include "Box2D/Box2D.h"

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/random.hpp"

#include "pixelboost/framework/engine.h"
#include "pixelboost/graphics/renderer/model/modelRenderer.h"
#include "pixelboost/logic/component/graphics/model.h"
#include "pixelboost/logic/component/physics/2d/physicsBody.h"
#include "pixelboost/logic/component/transform.h"
#include "pixelboost/logic/message/update.h"

#include "core/game.h"
#include "common/layers.h"
#include "gameplay/cog.h"

PB_DEFINE_ENTITY(Cog)

Cog::Cog(pb::Scene* scene, pb::Entity* parent, pb::DbEntity* creationEntity)
    : pb::Entity(scene, parent, creationEntity)
{
}

Cog::~Cog()
{
    UnregisterMessageHandler<pb::UpdateMessage>(pb::MessageHandler(this, &Cog::OnUpdate));
}

Cog* Cog::Initialise(glm::vec2 position, glm::vec2 speed)
{
    auto transform = CreateComponent<pb::TransformComponent>();
    transform->SetPosition(glm::vec3(position, 0));
    transform->SetRotation(glm::vec3(0.f, 0.f, glm::linearRand(0.f, 360.f)));

    auto physics = CreateComponent<pb::PhysicsBody2DComponent>();
    physics->Initialise(pb::PhysicsBody2DComponent::kBodyTypeDynamic, pb::PhysicsBody2DComponent::kBodyShapeCircle, glm::vec2(1.f, 1.f));
    physics->SetSensor(true);
    physics->GetBody()->SetLinearVelocity(b2Vec2(speed.x, speed.y));

    auto model = CreateComponent<pb::ModelComponent>();
    model->SetModel("/models/cog");
    model->SetMaterial("/materials/cog");
    model->SetLayer(kGraphicLayerPlayer);
    model->SetShader(Game::Instance()->GetLitShader());

    _Rotation = glm::linearRand(0.f, (float)M_PI*2.f);
    _RotationVector = glm::normalize(glm::vec3(glm::linearRand(0.f,1.f), glm::linearRand(0.f,1.f), glm::linearRand(0.f,1.f)));

    RegisterMessageHandler<pb::UpdateMessage>(pb::MessageHandler(this, &Cog::OnUpdate));
    
    return this;
}

void Cog::OnUpdate(const pb::Message& message)
{
    const pb::UpdateMessage& updateMessage = static_cast<const pb::UpdateMessage&>(message);
    
    _Rotation += updateMessage.GetGameDelta() * 180.f;

    GetComponent<pb::ModelComponent>()->SetLocalTransform(glm::rotate(glm::mat4x4(), _Rotation, _RotationVector));
}
