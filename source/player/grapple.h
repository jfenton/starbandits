#pragma once

#include "pixelboost/logic/entity.h"

namespace pb
{
    class Message;
    class Scene;
}

class Grapple : public pb::Entity
{
public:
    Grapple(pb::Scene* scene, pb::Uid playerId, glm::vec3 position, float rotation, float speed);
    ~Grapple();
    
public:
    virtual pb::Uid GetType() const;
    static pb::Uid GetStaticType();
    
    void OnCollision(const pb::Message& message);
    void OnUpdate(const pb::Message& message);
    
private:
    pb::Uid _PlayerId;
    b2Body* _CollisionBody;
    pb::Uid _CollisionObject;
    
    float _Life;
    float _Rotation;
};
