#include "pixelboost/logic/component/physics/physics.h"
#include "pixelboost/logic/message/physics/collision.h"
#include "pixelboost/logic/entity.h"

#include "gameplay/damage.h"
#include "gameplay/health.h"

HealthComponent::HealthComponent(pb::Entity* entity, int playerId, float health, float shields)
    : pb::Component(entity)
    , _Health(health)
    , _PlayerId(playerId)
    , _Shields(shields)
{
    GetParent()->RegisterMessageHandler<DamageMessage>(pb::Entity::MessageHandler(this, &HealthComponent::OnDamage));
    GetParent()->RegisterMessageHandler<pb::PhysicsCollisionMessage>(pb::Entity::MessageHandler(this, &HealthComponent::OnCollision));
}

HealthComponent::~HealthComponent()
{
    GetParent()->UnregisterMessageHandler<DamageMessage>(pb::Entity::MessageHandler(this, &HealthComponent::OnDamage));
    GetParent()->UnregisterMessageHandler<pb::PhysicsCollisionMessage>(pb::Entity::MessageHandler(this, &HealthComponent::OnCollision));
}

pb::Uid HealthComponent::GetType()
{
    return GetStaticType();
}

pb::Uid HealthComponent::GetStaticType()
{
    return pb::TypeHash("HealthComponent");
}

float HealthComponent::GetHealth()
{
    return _Health;
}

float HealthComponent::GetShields()
{
    return _Shields;
}

void HealthComponent::OnDamage(const pb::Message& message)
{
    const DamageMessage& damageMessage = static_cast<const DamageMessage&>(message);
    
    ModifyHealth(-damageMessage.GetDamage());
}

void HealthComponent::OnCollision(const pb::Message& message)
{
    const pb::PhysicsCollisionMessage& collisionMessage = static_cast<const pb::PhysicsCollisionMessage&>(message);
    
    DamageComponent* damage = collisionMessage.GetOtherComponent()->GetParent()->GetComponentByType<DamageComponent>();
    
    if (damage)
    {
        if (damage->GetPlayerId() != _PlayerId)
            ModifyHealth(-damage->GetKineticDamage());
    } else {
        ModifyHealth(-5.f);
    }
}

void HealthComponent::ModifyHealth(float health)
{
    _Health += health;
    
    if (_Health < 0.f)
    {
        GetParent()->Destroy();
    }
}
