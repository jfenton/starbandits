#pragma once

#include "pixelboost/input/joystickManager.h"
#include "pixelboost/input/keyboardManager.h"
#include "pixelboost/logic/entity.h"

namespace pb
{
    class Message;
    class Scene;
}

class PlayerInput
{
public:
    PlayerInput();
    
    glm::vec2 _Thrust;
    glm::vec2 _GrappleDirection;
    
    bool _Firing;
    bool _WasFiring;
    
    bool _Grapple;
    
    bool _BarrelLeft;
    bool _BarrelRight;
    
    bool _Boost;
};

class PlayerKeyboardInput : public PlayerInput, public pb::KeyboardHandler
{
public:
    PlayerKeyboardInput();
    ~PlayerKeyboardInput();
    
    virtual bool OnKeyDown(pb::KeyboardKey key, char character);
    virtual bool OnKeyUp(pb::KeyboardKey key, char character);
};

class PlayerJoystickInput : public PlayerInput, public pb::JoystickHandler
{
public:
    PlayerJoystickInput(int playerId);
    ~PlayerJoystickInput();
    
    virtual bool OnAxisChanged(int joystick, int stick, int axis, float value);
    virtual bool OnButtonDown(int joystick, int button);
    virtual bool OnButtonUp(int joystick, int button);
    
private:
    int _PlayerId;
};

class PlayerShip : public pb::Entity
{
public:
    PlayerShip(pb::Scene* scene, int playerId);
    ~PlayerShip();
    
public:
    virtual pb::Uid GetType() const;
    static pb::Uid GetStaticType();
    
    void OnUpdate(const pb::Message& message);
    
    float GetSpeedPercentage();
    
    void SetGrappleJoints(b2Joint* jointA, b2Joint* jointB);
    void SetGrappleObject(pb::Uid grappleObject);
    
    void ProcessGameBounds();
    
private:
    PlayerInput* _Input;
    
    float _BarrelCooldown;
    float _BoostPower;
    float _FiringDelay;
    int _PlayerId;
    float _Tilt;

    pb::Uid _GrappleId;
    pb::Uid _GrappleObject;
    b2Joint* _GrappleJointA;
    b2Joint* _GrappleJointB;
};
