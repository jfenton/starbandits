#pragma once

#include "pixelboost/logic/entity.h"

#include "player/projectile.h"

class Turret : public pb::Entity
{
public:
	Turret(pb::Scene* scene, glm::vec2 position, ProjectileType type);
    ~Turret();
    
    virtual pb::Uid GetType() const;
    static pb::Uid GetStaticType();
    
    void OnUpdate(const pb::Message& message);
    void OnHealthDepleted(const pb::Message& message);
    
private:
    glm::vec3 GetNearestPlayer();
    
    ProjectileType _Type;
    float _ShootTimer;
};
