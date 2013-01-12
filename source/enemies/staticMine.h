#pragma once

#include "pixelboost/logic/entity.h"

class StaticMine : public pb::Entity
{
public:
	StaticMine(pb::Scene* scene, glm::vec2 position);
    ~StaticMine();
    
    virtual pb::Uid GetType() const;
    static pb::Uid GetStaticType();
    
    void OnUpdate(const pb::Message& message);
    void OnHealthDepleted(const pb::Message& message);
    
private:
    bool _Active;
    float _Countdown;
    float _Rotation;
    float _DetectDistance;
};
