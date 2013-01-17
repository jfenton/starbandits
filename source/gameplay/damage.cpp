#include "gameplay/damage.h"

DamageComponent::DamageComponent(pb::Entity* entity, HealthType healthType, float damage)
	: pb::Component(entity)
	, _Damage(damage)
    , _HealthType(healthType)
{

}

DamageComponent::~DamageComponent()
{

}

pb::Uid DamageComponent::GetType()
{
	return GetStaticType();
}

pb::Uid DamageComponent::GetStaticType()
{
	return pb::TypeHash("DamageComponent");
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

DamageMessage::DamageMessage(pb::Entity* entity, pb::Component* component, float damage)
    : pb::Message(entity, component)
    , _Damage(damage)
{
    
}

DamageMessage::~DamageMessage()
{
    
}

pb::Uid DamageMessage::GetType() const
{
    return GetStaticType();
}

pb::Uid DamageMessage::GetStaticType()
{
    return pb::TypeHash("DamageHash");
}

float DamageMessage::GetDamage() const
{
    return _Damage;
}
