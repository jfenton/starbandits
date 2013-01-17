#pragma once

#include "pixelboost/logic/entity.h"

namespace pb
{
    class Message;
    class PrimitiveRenderableLine;
    class Scene;
}

class Grapple : public pb::Entity
{
public:
    Grapple(pb::Scene* scene, pb::Uid playerId, pb::Uid collisionId);
    ~Grapple();
    
public:
    virtual pb::Uid GetType() const;
    static pb::Uid GetStaticType();
    
    void OnCollision(const pb::Message& message);
    void OnUpdate(const pb::Message& message);
    
private:
    enum GrappleState
    {
        kStateFiring,
        kStateAttached,
    };
    
    pb::Uid _PlayerId;
    pb::Uid _CollisionId;
    
    pb::PrimitiveRenderableLine* _Renderable;
};
