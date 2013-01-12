#include "gameplay/damage.h"

DamageComponent::DamageComponent(pb::Entity* entity, int playerId, float energyDamage, float kineticDamage)
	: pb::Component(entity)
	, _EnergyDamage(energyDamage)
	, _KineticDamage(kineticDamage)
    , _PlayerId(playerId)
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

float DamageComponent::GetEnergyDamage() const
{
	return _EnergyDamage;
}

float DamageComponent::GetKineticDamage() const
{
	return _KineticDamage;
}

int DamageComponent::GetPlayerId() const
{
    return _PlayerId;
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
