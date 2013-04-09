#pragma once

#include "pixelboost/logic/entity.h"

class StaticMine : public pb::Entity
{
    PB_DECLARE_ENTITY
    
public:
	StaticMine(pb::Scene* scene, pb::Entity* entity, pb::DbEntity* creationEntity);
    ~StaticMine();
    
    void OnUpdate(const pb::Message& message);
    void OnHealthDepleted(const pb::Message& message);
    
private:
    bool _Active;
    float _Countdown;
    float _Rotation;
    float _DetectDistance;
};
