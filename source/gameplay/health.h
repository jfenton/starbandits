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
public:
    HealthComponent(pb::Entity* entity, HealthType healthType, float health, float shields);
    ~HealthComponent();
    
    virtual pb::Uid GetType();
    static pb::Uid GetStaticType();
    
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
public:
    HullHitMessage(pb::Entity* entity);
    ~HullHitMessage();
    
    pb::Uid GetType() const;
    static pb::Uid GetStaticType();
    
private:
};

class ShieldsHitMessage : public pb::Message
{
public:
    ShieldsHitMessage(pb::Entity* entity);
    ~ShieldsHitMessage();
    
    pb::Uid GetType() const;
    static pb::Uid GetStaticType();
    
private:
};

class HealthDepletedMessage : public pb::Message
{
public:
    HealthDepletedMessage(pb::Entity* entity);
    ~HealthDepletedMessage();
    
    pb::Uid GetType() const;
    static pb::Uid GetStaticType();
    
private:
};
