#pragma once

#include "pixelboost/logic/component.h"
#include "pixelboost/logic/message.h"

#include "gameplay/health.h"

class DamageComponent : public pb::Component
{
    PB_DECLARE_COMPONENT
    
public:
    DamageComponent(pb::Entity* entity);
    ~DamageComponent();
    
    void Initialise(HealthType healthType, float damage);
    
    float GetDamage() const;
    void SetDamage(float damage);
    
    HealthType GetHealthType() const;
    
private:
    float _Damage;
    HealthType _HealthType;
};

class DamageMessage : public pb::Message
{
    PB_DECLARE_MESSAGE
    
public:
    DamageMessage(pb::Entity* entity, pb::Component* component, float damage);
    ~DamageMessage();
    
    float GetDamage() const;
    
private:
    float _Damage;
};
