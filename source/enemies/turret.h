#pragma once

#include "pixelboost/logic/entity.h"

class Turret : public pb::Entity
{
public:
	Turret(pb::Scene* scene, glm::vec2 position);
    ~Turret();
    
    virtual pb::Uid GetType() const;
    static pb::Uid GetStaticType();
    
    void OnUpdate(const pb::Message& message);
    void OnHealthDepleted(const pb::Message& message);
    
private:
    float _ShootTimer;
};
