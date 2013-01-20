#pragma once

#include "pixelboost/logic/entity.h"

class Explosion : public pb::Entity
{
public:
    Explosion(pb::Scene* scene, glm::vec2 position, float power);
    ~Explosion();
    
    virtual pb::Uid GetType() const;
    static pb::Uid GetStaticType();
    
    void OnCollision(const pb::Message& message);
    void OnUpdate(const pb::Message& message);
    
private:
    float _Power;
    float _Size;
    float _Time;
};
