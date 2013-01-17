#pragma once

#include "pixelboost/logic/entity.h"

namespace pb
{
    class Message;
    class PrimitiveRenderableLine;
    class Scene;
}

class PlayerInput;

class Grapple : public pb::Entity
{
public:
    Grapple(pb::Scene* scene, pb::Uid playerId, PlayerInput* input, pb::Uid collisionId);
    ~Grapple();
    
public:
    virtual pb::Uid GetType() const;
    static pb::Uid GetStaticType();
    
    void OnCollision(const pb::Message& message);
    void OnUpdate(const pb::Message& message);
    
private:
    void UpdateData();
    float GetDistance();
    
    enum GrappleState
    {
        kStateFiring,
        kStateAttached,
    };
    
    pb::Uid _PlayerId;
    pb::Uid _CollisionId;
    
    PlayerInput* _Input;
    
    glm::vec3 _ObjectPosition;
    glm::vec3 _PlayerPosition;
    float _StartDistance;
    
    pb::PrimitiveRenderableLine* _Renderable;
};
