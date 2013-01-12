#pragma once

#include "pixelboost/logic/component.h"

class HealthComponent : pb::Component
{
public:
    HealthComponent(pb::Entity* entity, int playerId, float health, float shields);
    ~HealthComponent();
    
    virtual pb::Uid GetType();
    static pb::Uid GetStaticType();
    
    float GetHealth();
    float GetShields();
    
    void OnDamage(const pb::Message& message);
    void OnCollision(const pb::Message& message);
    
private:
    void ModifyHealth(float health);
    
    float _Health;
    float _Shields;
    
    int _PlayerId;
};
