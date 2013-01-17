#pragma once

#include "pixelboost/logic/component.h"

namespace pb
{
    class Message;
    class PrimitiveRenderableLine;
    class Scene;
}

class GrappleComponent : public pb::Component
{
public:
    GrappleComponent(pb::Entity* entity, pb::Uid shipEntity, pb::Uid connectedEntity);
    ~GrappleComponent();
    
public:
    virtual pb::Uid GetType();
    static pb::Uid GetStaticType();
    
    void OnUpdate(const pb::Message& message);
    
    void UpdatePosition();
    
private:
    pb::Uid _ConnectedEntity;
    pb::Uid _ShipEntity;
    pb::PrimitiveRenderableLine* _Renderable;
};
