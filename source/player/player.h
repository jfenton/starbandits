#pragma once

#include "pixelboost/input/joystickManager.h"
#include "pixelboost/input/keyboardManager.h"
#include "pixelboost/logic/entity.h"

#include "player/weapon/mount.h"

namespace pb
{
    class Message;
    class ParticleComponent;
    class Scene;
}

class PlayerInput
{
public:
    PlayerInput();
    virtual ~PlayerInput();
    
    glm::vec2 _Thrust;
    glm::vec2 _GrappleDirection;
    
    bool _FiringLeft;
    bool _FiringRight;

    bool _BarrelLeft;
    bool _BarrelRight;
    
    bool _Boost;
};

class PlayerKeyboardInput : public PlayerInput, public pb::KeyboardHandler
{
public:
    PlayerKeyboardInput();
    ~PlayerKeyboardInput();
    
    virtual bool OnKeyDown(pb::KeyboardKey key, pb::ModifierKeys modifier, char character);
    virtual bool OnKeyUp(pb::KeyboardKey key, pb::ModifierKeys modifier, char character);
    
private:
    void UpdateThrust();
    
    bool _KeyLeft;
    bool _KeyRight;
    bool _KeyUp;
    bool _KeyDown;
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

    float GetEnergy();
    void RemoveEnergy(float energy);
    float GetSpeedPercentage();
    float GetTilt();
    
    void ProcessGameBounds();
    void ProcessLighting();
    
private:
    void SetupEngineParticle(pb::ParticleComponent* particleComponent, glm::vec3 position, float scale);
    
    PlayerInput* _Input;
    
    pb::ParticleComponent* _EngineMain;
    pb::ParticleComponent* _EngineLeft;
    pb::ParticleComponent* _EngineRight;
    
    float _BarrelCooldown;
    float _Energy;
    bool _GrappleActive;
    int _PlayerId;
    float _Tilt;
    
    MountInfo _LeftMount;
    MountInfo _MissileMount;
    MountInfo _RightMount;
};
