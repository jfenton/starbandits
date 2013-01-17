#pragma once

#include "pixelboost/logic/component.h"
#include "pixelboost/logic/message.h"

#include "gameplay/health.h"

class DamageComponent : public pb::Component
{
public:
    DamageComponent(pb::Entity* entity, HealthType healthType, float damage);
    ~DamageComponent();
    
    virtual pb::Uid GetType();
    static pb::Uid GetStaticType();
    
    float GetDamage() const;
    void SetDamage(float damage);
    
    HealthType GetHealthType() const;
    
private:
    float _Damage;
    HealthType _HealthType;
};

class DamageMessage : public pb::Message
{
public:
    DamageMessage(pb::Entity* entity, pb::Component* component, float damage);
    ~DamageMessage();
    
    pb::Uid GetType() const;
    static pb::Uid GetStaticType();
    
    float GetDamage() const;
    
private:
    float _Damage;
};
