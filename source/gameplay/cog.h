#pragma once

#include "pixelboost/logic/entity.h"

class Cog : public pb::Entity
{
public:
    Cog(pb::Scene* scene, glm::vec2 position, glm::vec2 speed);
    ~Cog();
    
    virtual pb::Uid GetType() const;
    static pb::Uid GetStaticType();
    
    void OnUpdate(const pb::Message& message);
    
private:
    float _Rotation;
    glm::vec3 _RotationVector;
};
