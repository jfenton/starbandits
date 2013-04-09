#pragma once

#include "pixelboost/logic/component.h"
#include "pixelboost/logic/message.h"

enum HealthType
{
    kHealthTypeNone,
    kHealthTypeEnemy,
    kHealthTypePlayer,
};

class HealthComponent : public pb::Component
{
    PB_DECLARE_COMPONENT
    
public:
    HealthComponent(pb::Entity* entity);
    ~HealthComponent();
    
    void Initialise(HealthType healthType, float health, float shields);
    
    HealthType GetHealthType();
    
    float GetHealth();
    float GetShields();
    
    void OnCollision(const pb::Message& message);
    void OnDamage(const pb::Message& message);
    void OnUpdate(const pb::Message& message);
    
    void Damage(float damage);

private:
    float _Health;
    float _MaxShields;
    float _Shields;
    
    HealthType _HealthType;
};

class HullHitMessage : public pb::Message
{
    PB_DECLARE_MESSAGE
    
public:
    HullHitMessage(pb::Entity* entity);
    ~HullHitMessage();
    
private:
};

class ShieldsHitMessage : public pb::Message
{
    PB_DECLARE_MESSAGE
    
public:
    ShieldsHitMessage(pb::Entity* entity);
    ~ShieldsHitMessage();
    
private:
};

class HealthDepletedMessage : public pb::Message
{
    PB_DECLARE_MESSAGE
    
public:
    HealthDepletedMessage(pb::Entity* entity);
    ~HealthDepletedMessage();
    
private:
};
