#pragma once

#include "pixelboost/logic/entity.h"

class Cog : public pb::Entity
{
    PB_DECLARE_ENTITY
    
public:
    Cog(pb::Scene* scene, pb::Entity* parent, pb::DbEntity* creationEntity);
    ~Cog();
    
    Cog* Initialise(glm::vec2 position, glm::vec2 speed);
    
    void OnUpdate(const pb::Message& message);
    
private:
    float _Rotation;
    glm::vec3 _RotationVector;
};
