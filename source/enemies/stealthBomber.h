#pragma once

#include "pixelboost/logic/entity.h"

class StealthBomber : public pb::Entity
{
    PB_DECLARE_ENTITY
    
public:
	StealthBomber(pb::Scene* scene, pb::Entity* parent, pb::DbEntity* creationEntity);
    ~StealthBomber();
    
    StealthBomber* Initialise(glm::vec2 position);
    
    void OnUpdate(const pb::Message& message);
    void OnHealthDepleted(const pb::Message& message);
    
private:
    float _MineTimer;
};
