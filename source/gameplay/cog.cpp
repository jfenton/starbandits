#include "Box2D/Box2D.h"

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/random.hpp"

#include "pixelboost/framework/engine.h"
#include "pixelboost/graphics/renderer/model/modelRenderer.h"
#include "pixelboost/logic/component/graphics/model.h"
#include "pixelboost/logic/component/physics/2d/physicsBody.h"
#include "pixelboost/logic/component/transform/basic.h"
#include "pixelboost/logic/message/update.h"

#include "core/game.h"
#include "common/layers.h"
#include "gameplay/cog.h"

Cog::Cog(pb::Scene* scene, glm::vec2 position, glm::vec2 speed)
    : pb::Entity(scene, 0)
{
    pb::BasicTransformComponent* transform = new pb::BasicTransformComponent(this);
    transform->SetPosition(glm::vec3(position, 0));
    transform->SetRotation(glm::vec3(0.f, 0.f, glm::linearRand(0.f, 360.f)));
    
    pb::PhysicsBody2DComponent* physics = new pb::PhysicsBody2DComponent(this, pb::PhysicsBody2DComponent::kBodyTypeDynamic, pb::PhysicsBody2DComponent::kBodyShapeCircle, glm::vec2(1.f, 1.f));
    physics->SetSensor(true);
    physics->GetBody()->SetLinearVelocity(b2Vec2(speed.x, speed.y));

    pb::ModelComponent* model = new pb::ModelComponent(this,
                                   pb::Engine::Instance()->GetModelRenderer()->GetModel("cog"),
                                   pb::Engine::Instance()->GetModelRenderer()->GetTexture("cog_DIFF"));
    model->SetLayer(kGraphicLayerPlayer);
    model->SetShader(Game::Instance()->GetLitShader());

    _Rotation = glm::linearRand(0.f, (float)M_PI*2.f);
    _RotationVector = glm::normalize(glm::vec3(glm::linearRand(0.f,1.f), glm::linearRand(0.f,1.f), glm::linearRand(0.f,1.f)));
    
    RegisterMessageHandler<pb::UpdateMessage>(MessageHandler(this, &Cog::OnUpdate));
}

Cog::~Cog()
{
    UnregisterMessageHandler<pb::UpdateMessage>(MessageHandler(this, &Cog::OnUpdate));
}

pb::Uid Cog::GetType() const
{
    return GetStaticType();
}

pb::Uid Cog::GetStaticType()
{
    return pb::TypeHash("Cog");
}

void Cog::OnUpdate(const pb::Message& message)
{
    const pb::UpdateMessage& updateMessage = static_cast<const pb::UpdateMessage&>(message);
    
    _Rotation += updateMessage.GetDelta() * 180.f;

    GetComponentByType<pb::ModelComponent>()->SetLocalTransform(glm::rotate(glm::mat4x4(), _Rotation, _RotationVector));
}
