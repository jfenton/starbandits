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
    PB_DECLARE_COMPONENT
    
public:
    MissileComponent(pb::Entity* entity);
    ~MissileComponent();
    
    void Initialise(PlayerInput* input, const MountInfo& mountInfo);
    
public:
    virtual void OnUpdate(const pb::Message& message);
    
private:
    bool _Fired;
    PlayerInput* _Input;
    MountInfo _MountInfo;
};
