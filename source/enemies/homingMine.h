#pragma once

#include "pixelboost/logic/entity.h"

class HomingMine : public pb::Entity
{
public:
	HomingMine(pb::Scene* scene, glm::vec2 position);
    ~HomingMine();
    
    virtual pb::Uid GetType() const;
    static pb::Uid GetStaticType();
    
    void OnUpdate(const pb::Message& message);
    
private:
    float _Rotation;
    float _DetectDistance;
};
