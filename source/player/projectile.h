#pragma once

#include "pixelboost/logic/entity.h"

#include "gameplay/health.h"

namespace pb
{
    class Message;
    class Scene;
}

enum ProjectileType
{
    kProjectileTypeLaser,
    kProjectileTypeBeam,
    kProjectileTypeHoming,
};

class Projectile : public pb::Entity
{
public:
	Projectile(pb::Scene* scene, HealthType healthType, ProjectileType behaviour, glm::vec3 position, float rotation, float speed, float damage);
    ~Projectile();
    
public:
    virtual pb::Uid GetType() const;
    static pb::Uid GetStaticType();
    
private:
    void OnCollision(const pb::Message& message);
    void OnUpdate(const pb::Message& message);
    
    void UpdateProjectile();
    void SetTarget();
    
private:
    ProjectileType _Type;
    float _Life;
    float _Speed;
    pb::Uid _Target;
};
