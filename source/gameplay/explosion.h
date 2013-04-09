#pragma once

#include "pixelboost/logic/entity.h"

class Explosion : public pb::Entity
{
    PB_DECLARE_ENTITY
    
public:
    Explosion(pb::Scene* scene, pb::Entity* parent, pb::DbEntity* creationEntity);
    ~Explosion();
    
    void Initialise(glm::vec2 position, float power);
    
    void OnUpdate(const pb::Message& message);
    
private:
    float _Power;
    float _Size;
    float _Time;
};
