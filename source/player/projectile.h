#pragma once

#include "pixelboost/logic/entity.h"

#include "gameplay/health.h"

namespace pb
{
    class Message;
    class Scene;
}

class Projectile : public pb::Entity
{
public:
    Projectile(pb::Scene* scene, HealthType healthType, glm::vec3 position, float rotation, float speed, float damage);
    ~Projectile();
    
public:
    virtual pb::Uid GetType() const;
    static pb::Uid GetStaticType();
    
    void OnCollision(const pb::Message& message);
    void OnUpdate(const pb::Message& message);
    
private:
    float _Life;
};
