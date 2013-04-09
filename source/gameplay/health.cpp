#include "pixelboost/logic/component/physics/physics.h"
#include "pixelboost/logic/message/physics/collision.h"
#include "pixelboost/logic/message/update.h"
#include "pixelboost/logic/entity.h"
#include "pixelboost/logic/scene.h"

#include "gameplay/damage.h"
#include "gameplay/health.h"
#include "player/player.h"

PB_DEFINE_COMPONENT(HealthComponent)

HealthComponent::HealthComponent(pb::Entity* entity)
    : pb::Component(entity)
    , _Health(0)
    , _MaxShields(0)
    , _Shields(0)
    , _HealthType(kHealthTypeNone)
{
    
}

void HealthComponent::Initialise(HealthType healthType, float health, float shields)
{
    _Health = health;
    _HealthType = healthType;
    _MaxShields = shields;
    _Shields = shields;
    
    GetEntity()->RegisterMessageHandler<DamageMessage>(pb::MessageHandler(this, &HealthComponent::OnDamage));
    GetEntity()->RegisterMessageHandler<pb::PhysicsCollisionMessage>(pb::MessageHandler(this, &HealthComponent::OnCollision));
    GetEntity()->RegisterMessageHandler<pb::UpdateMessage>(pb::MessageHandler(this, &HealthComponent::OnUpdate));
}

HealthComponent::~HealthComponent()
{
    GetEntity()->UnregisterMessageHandler<DamageMessage>(pb::MessageHandler(this, &HealthComponent::OnDamage));
    GetEntity()->UnregisterMessageHandler<pb::PhysicsCollisionMessage>(pb::MessageHandler(this, &HealthComponent::OnCollision));
    GetEntity()->UnregisterMessageHandler<pb::UpdateMessage>(pb::MessageHandler(this, &HealthComponent::OnUpdate));
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
    
    DamageComponent* damage = collisionMessage.GetOtherComponent()->GetEntity()->GetComponent<DamageComponent>();
    
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
        rechargeAmount = glm::min(_MaxShields-_Shields, glm::max(((_MaxShields-_Shields)/(_MaxShields/4.f)),1.f)*updateMessage.GetGameDelta());
    }

    if (GetEntity()->GetType() == PlayerShip::GetStaticType())
    {
        float rechargeCost = rechargeAmount * 2.f;
        
        PlayerShip* ship = static_cast<PlayerShip*>(GetEntity());

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
        HealthDepletedMessage healthDepleted(GetEntity());
        GetScene()->SendMessage(GetEntityUid(), healthDepleted);
        GetEntity()->Destroy();
    }
}

void HealthComponent::Damage(float damage)
{
    if (_Shields > 0)
    {
        _Shields -= damage;
        
        ShieldsHitMessage shieldsHit(GetEntity());
        GetScene()->SendMessage(GetEntityUid(), shieldsHit);
    } else {
        _Health -= damage;
        
        HullHitMessage hullHit(GetEntity());
        GetScene()->SendMessage(GetEntityUid(), hullHit);
    }
    
    if (_Shields < 0.f)
    {
        _Health += _Shields;
        _Shields = 0.f;
        
        HullHitMessage hullHit(GetEntity());
        GetScene()->SendMessage(GetEntityUid(), hullHit);
    }
    
    _Health = glm::max(_Health, 0.f);
}

PB_DEFINE_MESSAGE(HealthDepletedMessage)

HealthDepletedMessage::HealthDepletedMessage(pb::Entity* entity)
    : pb::Message(entity, 0)
{
    
}

HealthDepletedMessage::~HealthDepletedMessage()
{
    
}

PB_DEFINE_MESSAGE(HullHitMessage)

HullHitMessage::HullHitMessage(pb::Entity* entity)
    : pb::Message(entity, 0)
{
    
}

HullHitMessage::~HullHitMessage()
{
    
}

PB_DEFINE_MESSAGE(ShieldsHitMessage)

ShieldsHitMessage::ShieldsHitMessage(pb::Entity* entity)
    : pb::Message(entity, 0)
{
    
}

ShieldsHitMessage::~ShieldsHitMessage()
{
    
}
