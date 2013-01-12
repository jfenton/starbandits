#include "pixelboost/logic/component/graphics/ellipse.h"
#include "pixelboost/logic/component/physics/2d/physicsBody.h"
#include "pixelboost/logic/component/transform/basic.h"
#include "pixelboost/logic/message/physics/collision.h"
#include "pixelboost/logic/message/update.h"

#include "gameplay/damage.h"
#include "gameplay/explosion.h"

Explosion::Explosion(pb::Scene* scene, glm::vec2 position, float power)
    : pb::Entity(scene, 0)
    , _Power(power)
    , _Size(0)
{
    pb::BasicTransformComponent* transform = new pb::BasicTransformComponent(this);
    transform->SetPosition(glm::vec3(position, 0));
    
    pb::EllipseComponent* ellipse = new pb::EllipseComponent(this);
    ellipse->SetSize(glm::vec2(0,0));
    ellipse->SetSolid(true);
    ellipse->SetColor(glm::vec4(205.f/255.f,112.f/255.f,6.f/255.f,1));
    
    new DamageComponent(this, kHealthTypeNone, 0.f, 5.f);
    
    pb::PhysicsBody2DComponent* physics = new pb::PhysicsBody2DComponent(this, pb::PhysicsBody2DComponent::kBodyTypeDynamic, pb::PhysicsBody2DComponent::kBodyShapeCircle, glm::vec2(_Power/2.f, _Power/2.f));

    physics->SetSensor(true);
    
    RegisterMessageHandler<pb::PhysicsCollisionMessage>(MessageHandler(this, &Explosion::OnCollision));
    RegisterMessageHandler<pb::UpdateMessage>(MessageHandler(this, &Explosion::OnUpdate));
}

Explosion::~Explosion()
{
    UnregisterMessageHandler<pb::PhysicsCollisionMessage>(MessageHandler(this, &Explosion::OnCollision));
    UnregisterMessageHandler<pb::UpdateMessage>(MessageHandler(this, &Explosion::OnUpdate));
}

pb::Uid Explosion::GetType() const
{
    return GetStaticType();
}

pb::Uid Explosion::GetStaticType()
{
    return pb::TypeHash("Explosion");
}

void Explosion::OnCollision(const pb::Message& message)
{
    
}

void Explosion::OnUpdate(const pb::Message& message)
{
    const pb::UpdateMessage& updateMessage = static_cast<const pb::UpdateMessage&>(message);
    
    _Size += (updateMessage.GetDelta()*15.f);
    
    GetComponentByType<pb::EllipseComponent>()->SetSize(glm::vec2(_Size, _Size));
    
    if (_Size > _Power)
        Destroy();
}
