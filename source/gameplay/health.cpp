#include "pixelboost/logic/component/physics/physics.h"
#include "pixelboost/logic/message/physics/collision.h"
#include "pixelboost/logic/message/update.h"
#include "pixelboost/logic/entity.h"
#include "pixelboost/logic/scene.h"

#include "gameplay/damage.h"
#include "gameplay/health.h"

HealthComponent::HealthComponent(pb::Entity* entity, HealthType healthType, float health, float shields)
    : pb::Component(entity)
    , _Health(health)
    , _HealthType(healthType)
    , _Shields(shields)
{
    GetParent()->RegisterMessageHandler<DamageMessage>(pb::Entity::MessageHandler(this, &HealthComponent::OnDamage));
    GetParent()->RegisterMessageHandler<pb::PhysicsCollisionMessage>(pb::Entity::MessageHandler(this, &HealthComponent::OnCollision));
    GetParent()->RegisterMessageHandler<pb::UpdateMessage>(pb::Entity::MessageHandler(this, &HealthComponent::OnUpdate));
}

HealthComponent::~HealthComponent()
{
    GetParent()->UnregisterMessageHandler<DamageMessage>(pb::Entity::MessageHandler(this, &HealthComponent::OnDamage));
    GetParent()->UnregisterMessageHandler<pb::PhysicsCollisionMessage>(pb::Entity::MessageHandler(this, &HealthComponent::OnCollision));
    GetParent()->UnregisterMessageHandler<pb::UpdateMessage>(pb::Entity::MessageHandler(this, &HealthComponent::OnUpdate));
}

pb::Uid HealthComponent::GetType()
{
    return GetStaticType();
}

pb::Uid HealthComponent::GetStaticType()
{
    return pb::TypeHash("HealthComponent");
}

HealthType HealthComponent::GetHealthType()
{
    return _HealthType;
}

float HealthComponent::GetHealth()
{
    return _Health;
}

float HealthComponent::GetShields()
{
    return _Shields;
}

void HealthComponent::OnCollision(const pb::Message& message)
{
    const pb::PhysicsCollisionMessage& collisionMessage = static_cast<const pb::PhysicsCollisionMessage&>(message);
    
    DamageComponent* damage = collisionMessage.GetOtherComponent()->GetParent()->GetComponentByType<DamageComponent>();
    
    if (damage)
    {
        if (damage->GetHealthType() != _HealthType)
            ModifyHealth(-damage->GetKineticDamage());
    } else {
        //ModifyHealth(-5.f);
    }
}

void HealthComponent::OnDamage(const pb::Message& message)
{
    const DamageMessage& damageMessage = static_cast<const DamageMessage&>(message);
    
    ModifyHealth(-damageMessage.GetDamage());
}

void HealthComponent::OnUpdate(const pb::Message& message)
{
    if (_Health <= 0.f)
    {
        HealthDepletedMessage healthDepleted(GetParent());
        GetScene()->SendMessage(GetParentUid(), healthDepleted);
        GetParent()->Destroy();
    }
}

void HealthComponent::ModifyHealth(float health)
{
    _Health += health;
}

HealthDepletedMessage::HealthDepletedMessage(pb::Entity* entity)
    : pb::Message(entity, 0)
{
    
}

HealthDepletedMessage::~HealthDepletedMessage()
{
    
}

pb::Uid HealthDepletedMessage::GetType() const
{
    return GetStaticType();
}

pb::Uid HealthDepletedMessage::GetStaticType()
{
    return pb::TypeHash("HealthDepletedMessage");
}
