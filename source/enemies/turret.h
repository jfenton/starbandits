#pragma once

#include "pixelboost/logic/entity.h"

#include "player/projectile.h"

class Turret : public pb::Entity
{
    PB_DECLARE_ENTITY
    
public:
	Turret(pb::Scene* scene, pb::Entity* parent, pb::DbEntity* creationEntity);
    ~Turret();
    
    void Initialise(glm::vec2 position, ProjectileType type);
    
    void OnUpdate(const pb::Message& message);
    void OnHealthDepleted(const pb::Message& message);
    
private:
    glm::vec3 GetNearestPlayer();
    
    ProjectileType _Type;
    float _ShootTimer;
};
