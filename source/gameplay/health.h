#pragma once

#include "pixelboost/logic/component.h"

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
    
    void ModifyHealth(float health);

private:
    float _Health;
    float _Shields;
    
    HealthType _HealthType;
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
