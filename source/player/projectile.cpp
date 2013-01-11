#include "Box2D/Box2D.h"
#include "glm/gtc/matrix_transform.hpp"

#include "pixelboost/framework/engine.h"
#include "pixelboost/logic/component/graphics/rectangle.h"
#include "pixelboost/logic/component/physics/2d/physicsBody.h"
#include "pixelboost/logic/component/transform/basic.h"
#include "pixelboost/logic/message/physics/collision.h"
#include "pixelboost/logic/message/update.h"

#include "player/projectile.h"

Projectile::Projectile(pb::Scene* scene, glm::vec3 position, float rotation, float speed)
    : pb::Entity(scene, 0)
    , _Life(5.f)
{
    pb::BasicTransformComponent* transform = new pb::BasicTransformComponent(this);
    transform->SetRotation(glm::vec3(0,0,glm::degrees(rotation)));
    transform->SetPosition(position);
    
    pb::RectangleComponent* rectangle = new pb::RectangleComponent(this);
    rectangle->SetSize(glm::vec2(0.1f, 0.5f));
    rectangle->SetColor(glm::vec4(0.9,0.3,0.3,1));
    rectangle->SetSolid(true);
    rectangle->SetLayer(1);
    
    pb::PhysicsBody2DComponent* physics = new pb::PhysicsBody2DComponent(this, pb::PhysicsBody2DComponent::kBodyTypeDynamic, pb::PhysicsBody2DComponent::kBodyShapeRect, glm::vec2(0.1,0.3));
    physics->SetSensor(true);
    physics->GetBody()->SetBullet(true);
    physics->GetBody()->SetLinearVelocity(b2Vec2(cos(rotation+glm::radians(90.f))*speed, sin(rotation+glm::radians(90.f))*speed));
    
    RegisterMessageHandler<pb::PhysicsCollisionMessage>(MessageHandler(this, &Projectile::OnCollision));
    RegisterMessageHandler<pb::UpdateMessage>(MessageHandler(this, &Projectile::OnUpdate));
}

Projectile::~Projectile()
{
    UnregisterMessageHandler<pb::PhysicsCollisionMessage>(MessageHandler(this, &Projectile::OnCollision));
    UnregisterMessageHandler<pb::UpdateMessage>(MessageHandler(this, &Projectile::OnUpdate));
}

pb::Uid Projectile::GetType() const
{
    return GetStaticType();
}

pb::Uid Projectile::GetStaticType()
{
    return pb::TypeHash("Projectile");
}

void Projectile::OnCollision(const pb::Message& message)
{
    const pb::PhysicsCollisionMessage& collisionMessage = static_cast<const pb::PhysicsCollisionMessage&>(message);
    
    if (collisionMessage.GetOtherComponent()->GetParent()->GetType() == pb::TypeHash("Asteroid"))
    {
        collisionMessage.GetOtherComponent()->GetParent()->Destroy();
    }
}

void Projectile::OnUpdate(const pb::Message& message)
{
    const pb::UpdateMessage& updateMessage = static_cast<const pb::UpdateMessage&>(message);
    
    _Life -= updateMessage.GetDelta();
    
    if (_Life <= 0.f)
        Destroy();
}
