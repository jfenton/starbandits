
#include "Box2D/Box2D.h"
#include "glm/gtc/matrix_transform.hpp"

#include "pixelboost/framework/engine.h"
#include "pixelboost/logic/component/graphics/sprite.h"
#include "pixelboost/logic/component/physics/2d/physicsBody.h"
#include "pixelboost/logic/component/transform/basic.h"
#include "pixelboost/logic/message/physics/collision.h"
#include "pixelboost/logic/message/update.h"
#include "pixelboost/logic/scene.h"

#include "common/layers.h"
#include "gameplay/damage.h"
#include "gameplay/health.h"
#include "player/player.h"
#include "player/projectile.h"

Projectile::Projectile(pb::Scene* scene, HealthType healthType, glm::vec3 position, float rotation, float speed, float damage)
    : pb::Entity(scene, 0)
    , _Life(2.f)
{
    pb::BasicTransformComponent* transform = new pb::BasicTransformComponent(this);
    transform->SetRotation(glm::vec3(0,0,glm::degrees(rotation)));
    transform->SetPosition(position);
    
    pb::SpriteComponent* sprite = new pb::SpriteComponent(this, "");
    if (healthType == kHealthTypeEnemy)
    {
        sprite->SetSprite("laser_red");
    } else {
        sprite->SetSprite("laser_orange");
    }
    glm::mat4x4 spriteTransform;
    spriteTransform = glm::translate(spriteTransform, glm::vec3(0,1,0));
    spriteTransform = glm::rotate(spriteTransform, -90.f, glm::vec3(0,0,1));
    
    if (healthType == kHealthTypeEnemy)
    {
        spriteTransform = glm::scale(spriteTransform, glm::vec3(1.5,1.5,1));
    }
    
    sprite->SetLocalTransform(spriteTransform);
    sprite->SetLayer(kGraphicLayerProjectiles);
    
    pb::PhysicsBody2DComponent* physics = new pb::PhysicsBody2DComponent(this, pb::PhysicsBody2DComponent::kBodyTypeDynamic, pb::PhysicsBody2DComponent::kBodyShapeRect, glm::vec2(0.08,0.4));
    physics->SetSensor(true);
    physics->GetBody()->SetBullet(true);
    physics->GetBody()->SetLinearVelocity(b2Vec2(cos(rotation+glm::radians(90.f))*speed, sin(rotation+glm::radians(90.f))*speed));
    
    new DamageComponent(this, healthType, 5.f);
    
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
    
    HealthComponent* health = collisionMessage.GetOtherComponent()->GetParent()->GetComponentByType<HealthComponent>();
    
    pb::Uid type = collisionMessage.GetOtherComponent()->GetParent()->GetType();
    if (type == GetStaticType() || type == pb::TypeHash("Explosion"))
        return;
    
    if (!health || GetComponentByType<DamageComponent>()->GetHealthType() != health->GetHealthType())
        Destroy();
}

void Projectile::OnUpdate(const pb::Message& message)
{
    const pb::UpdateMessage& updateMessage = static_cast<const pb::UpdateMessage&>(message);
    
    _Life -= updateMessage.GetDelta();
    
    if (_Life <= 0.f)
        Destroy();
}
