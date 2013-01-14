#pragma once

#include "pixelboost/logic/entity.h"

class StealthBomber : public pb::Entity
{
public:
	StealthBomber(pb::Scene* scene, glm::vec2 position);
    ~StealthBomber();
    
    virtual pb::Uid GetType() const;
    static pb::Uid GetStaticType();
    
    void OnUpdate(const pb::Message& message);
    void OnHealthDepleted(const pb::Message& message);
    
private:
    float _MineTimer;
};
