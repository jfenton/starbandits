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
    PB_DECLARE_ENTITY
    
public:
	Projectile(pb::Scene* scene, pb::Entity* parent, pb::DbEntity* creationEntity);
    ~Projectile();
    
    void Initialise(HealthType healthType, ProjectileType behaviour, glm::vec3 position, float rotation, float speed, float damage);
    
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
