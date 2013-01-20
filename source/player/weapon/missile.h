#pragma once

#include "pixelboost/logic/component.h"

#include "player/weapon/mount.h"

namespace pb
{
    class Message;
    class ModelRenderable;
}

class PlayerInput;

class MissileComponent : public pb::Component
{
public:
    MissileComponent(pb::Entity* entity, PlayerInput* input, const MountInfo& mountInfo);
    ~MissileComponent();
    
public:
    virtual pb::Uid GetType();
    static pb::Uid GetStaticType();
    
    virtual void OnTransformChanged(const pb::Message& message);
    virtual void OnUpdate(const pb::Message& message);
    
private:
    void UpdateTransform();
    
    bool _Fired;
    pb::ModelRenderable* _Renderable;
    PlayerInput* _Input;
    const MountInfo& _MountInfo;
};
