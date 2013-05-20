#pragma once

#include "pixelboost/logic/component.h"

#include "player/weapon/mount.h"

namespace pb
{
    class Message;
}

class PlayerInput;

class LaserComponent : public pb::Component
{
    PB_DECLARE_COMPONENT
    
public:
    LaserComponent(pb::Entity* entity);
    ~LaserComponent();
    
    void Initialise(PlayerInput* input, const MountInfo& mountInfo);
    
public:
    virtual void OnUpdate(const pb::Message& message);
    
private:
    float _FiringDelay;
    float _SoundDelay;
    
    PlayerInput* _Input;
    MountInfo _MountInfo;
};
