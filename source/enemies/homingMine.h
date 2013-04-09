#pragma once

#include "pixelboost/logic/entity.h"

class HomingMine : public pb::Entity
{
    PB_DECLARE_ENTITY
    
public:
	HomingMine(pb::Scene* scene, pb::Entity* entity, pb::DbEntity* creationEntity);
    ~HomingMine();
    
    void Initialise(glm::vec2 position);
    
    void OnUpdate(const pb::Message& message);
    void OnHealthDepleted(const pb::Message& message);
    
private:
    float _Fuse;
    float _Rotation;
    float _DetectDistance;
    pb::Uid _PlayerId;
};
