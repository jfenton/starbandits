#include "pixelboost/logic/component/physics/physics.h"
#include "pixelboost/logic/message/physics/collision.h"
#include "pixelboost/logic/message/update.h"
#include "pixelboost/logic/entity.h"
#include "pixelboost/logic/scene.h"

#include "gameplay/damage.h"
#include "gameplay/health.h"
#include "player/player.h"

HealthComponent::HealthComponent(pb::Entity* entity, HealthType healthType, float health, float shields)
    : pb::Component(entity)
    , _Health(health)
    , _HealthType(healthType)
    , _MaxShields(shields)
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
            Damage(damage->GetDamage());
    } else {
        //ModifyHealth(-5.f);
    }
}

void HealthComponent::OnDamage(const pb::Message& message)
{
    const DamageMessage& damageMessage = static_cast<const DamageMessage&>(message);
    
    Damage(damageMessage.GetDamage());
}

void HealthComponent::OnUpdate(const pb::Message& message)
{
    const pb::UpdateMessage& updateMessage = static_cast<const pb::UpdateMessage&>(message);
    
    float rechargeAmount = 0.f;
    if (_Shields < _MaxShields)
    {
        rechargeAmount = glm::min(_MaxShields-_Shields, glm::max(((_MaxShields-_Shields)/(_MaxShields/4.f)),1.f)*updateMessage.GetDelta());
    }

    if (GetParent()->GetType() == PlayerShip::GetStaticType())
    {
        float rechargeCost = rechargeAmount * 2.f;
        
        PlayerShip* ship = static_cast<PlayerShip*>(GetParent());

        if (ship->GetEnergy() > rechargeCost)
        {
            _Shields += rechargeAmount;
            ship->RemoveEnergy(rechargeCost);
        }
    } else {
        _Shields += rechargeAmount;
    }
    
    _Shields = glm::min(_Shields, _MaxShields);
    
    if (_Health <= 0.f)
    {
        HealthDepletedMessage healthDepleted(GetParent());
        GetScene()->SendMessage(GetParentUid(), healthDepleted);
        GetParent()->Destroy();
    }
}

void HealthComponent::Damage(float damage)
{
    if (_Shields > 0)
    {
        _Shields -= damage;
    } else {
        _Health -= damage;
    }
    
    if (_Shields < 0.f)
    {
        _Health += _Shields;
        _Shields = 0.f;
    }
    
    _Health = glm::max(_Health, 0.f);
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
