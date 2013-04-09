#include "gameplay/damage.h"

PB_DEFINE_COMPONENT(DamageComponent)

DamageComponent::DamageComponent(pb::Entity* entity)
	: pb::Component(entity)
{

}

DamageComponent::~DamageComponent()
{

}

void DamageComponent::Initialise(HealthType healthType, float damage)
{
    _Damage = damage;
    _HealthType = healthType;
}

float DamageComponent::GetDamage() const
{
	return _Damage;
}

void DamageComponent::SetDamage(float damage)
{
    _Damage = damage;
}

HealthType DamageComponent::GetHealthType() const
{
    return _HealthType;
}

PB_DEFINE_MESSAGE(DamageMessage)

DamageMessage::DamageMessage(pb::Entity* entity, pb::Component* component, float damage)
    : pb::Message(entity, component)
    , _Damage(damage)
{
    
}

DamageMessage::~DamageMessage()
{
    
}

float DamageMessage::GetDamage() const
{
    return _Damage;
}
