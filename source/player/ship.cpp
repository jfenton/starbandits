#include "Box2D/Box2D.h"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/noise.hpp"

#include "pixelboost/framework/engine.h"
#include "pixelboost/graphics/renderer/model/modelRenderer.h"
#include "pixelboost/logic/component/graphics/model.h"
#include "pixelboost/logic/component/physics/2d/physicsBody.h"
#include "pixelboost/logic/component/transform/basic.h"
#include "pixelboost/logic/message/update.h"

#include "player/projectile.h"
#include "player/ship.h"

PlayerInput::PlayerInput()
    : _Firing(false)
    , _BarrelLeft(false)
    , _BarrelRight(false)
    , _Boost(false)
{
    
}

PlayerKeyboardInput::PlayerKeyboardInput()
{
    pb::Engine::Instance()->GetKeyboardManager()->AddHandler(this);
}

PlayerKeyboardInput::~PlayerKeyboardInput()
{
    pb::Engine::Instance()->GetKeyboardManager()->RemoveHandler(this);
}

bool PlayerKeyboardInput::OnKeyDown(pb::KeyboardKey key, char character)
{
    if (key == pb::kKeyboardKeyCharacter)
    {
        switch (character)
        {
            case 'w':
                _Thrust.y = 1.f;
                break;
            case 'a':
                _Thrust.x = -1.f;
                break;
            case 'd':
                _Thrust.x = 1.f;
                break;
            case 's':
                _Thrust.y = -1.f;
                break;
        }
    }
    
    if (key == pb::kKeyboardKeySpace)
    {
        _Firing = true;
    }
    
    return false;
}


bool PlayerKeyboardInput::OnKeyUp(pb::KeyboardKey key, char character)
{
    if (key == pb::kKeyboardKeyCharacter)
    {
        switch (character)
        {
            case 'w':
                _Thrust.y = 0.f;
                break;
            case 'a':
                _Thrust.x = 0.f;
                break;
            case 'd':
                _Thrust.x = 0.f;
                break;
            case 's':
                _Thrust.y = 0.f;
                break;
        }
    }
    
    if (key == pb::kKeyboardKeySpace)
    {
        _Firing = false;
    }
    
    return false;
}

PlayerJoystickInput::PlayerJoystickInput(int playerId)
    : _PlayerId(playerId)
{
    pb::Engine::Instance()->GetJoystickManager()->AddHandler(this);
}

PlayerJoystickInput::~PlayerJoystickInput()
{
    pb::Engine::Instance()->GetJoystickManager()->RemoveHandler(this);
}

bool PlayerJoystickInput::OnAxisChanged(int joystick, int stick, int axis, float value)
{
    if (joystick != _PlayerId)
        return false;
    
    if (stick == 0)
    {
        if (axis == 0)
        {
            if (glm::abs(value) > 0.25f)
            {
                _Thrust.x = value;
            } else {
                _Thrust.x = 0.f;
            }
        } else {
            if (glm::abs(value) > 0.25f)
            {
                _Thrust.y = -value;
            } else {
                _Thrust.y = 0.f;
            }
        }
    } else if (stick == 1)
    {
        if (axis == 0)
        {
            if (glm::abs(value) > 0.25f)
            {
                _Grapple.x = value;
            } else {
                _Grapple.x = 0.f;
            }
        } else {
            if (glm::abs(value) > 0.25f)
            {
                _Grapple.y = value;
            } else {
                _Grapple.y = 0.f;
            }
        }
    } else if (stick == 2)
    {
        if (axis == 1)
            _Firing = (value > 0);
    }
    
    return false;
}

bool PlayerJoystickInput::OnButtonDown(int joystick, int button)
{
    if (joystick != _PlayerId)
        return false;
    
    if (button == 5)
    {
        _BarrelRight = true;
    } else if (button == 4)
    {
        _BarrelLeft = true;
    }
    
    if (button == 0)
    {
        _Boost = true;
    }
    
    return false;
}

bool PlayerJoystickInput::OnButtonUp(int joystick, int button)
{
    if (joystick != _PlayerId)
        return false;
    
    if (button == 5)
    {
        _BarrelRight = false;
    } else if (button == 4)
    {
        _BarrelLeft = false;
    }
    
    if (button == 0)
    {
        _Boost = false;
    }

    return false;
}

PlayerShip::PlayerShip(pb::Scene* scene)
    : pb::Entity(scene, 0)
    , _BarrelCooldown(0)
    , _BoostPower(1.5)
    , _FiringDelay(0)
    , _Tilt(0)
{
    new pb::BasicTransformComponent(this);
    
    pb::ModelComponent* model = new pb::ModelComponent(this,
                           pb::Engine::Instance()->GetModelRenderer()->GetModel("ship"),
                           pb::Engine::Instance()->GetModelRenderer()->GetTexture("ship"));
    model->SetLocalTransform(glm::scale(glm::mat4x4(), glm::vec3(0.5, 0.5, 0.5)));
    model->SetLayer(1);
    
    pb::PhysicsBody2DComponent* physics = new pb::PhysicsBody2DComponent(this, pb::PhysicsBody2DComponent::kBodyTypeDynamic, pb::PhysicsBody2DComponent::kBodyShapeCircle, glm::vec2(1,1));
    
    physics->GetBody()->SetAngularDamping(1.f);
    
    _Input = new PlayerJoystickInput(0);
    
    RegisterMessageHandler<pb::UpdateMessage>(MessageHandler(this, &PlayerShip::OnUpdate));
}

PlayerShip::~PlayerShip()
{
    UnregisterMessageHandler<pb::UpdateMessage>(MessageHandler(this, &PlayerShip::OnUpdate));
    
    delete _Input;
}

pb::Uid PlayerShip::GetType() const
{
    return GetStaticType();
}

pb::Uid PlayerShip::GetStaticType()
{
    return pb::TypeHash("PlayerShip");
}

void PlayerShip::OnUpdate(const pb::Message& message)
{
    float barrelControlLock = 1.25f;
    float barrelLength = 1.5f;
    float barrelCooldown = 4.f;
    
    bool controlLocked = _BarrelCooldown-(barrelCooldown-barrelControlLock) > 0.f;
    
    float enginePower = 200.f;
    
    const pb::UpdateMessage& updateMessage = static_cast<const pb::UpdateMessage&>(message);
    float thrustPower = enginePower * updateMessage.GetDelta();
    
    b2Body* body = GetComponentByType<pb::PhysicsBody2DComponent>()->GetBody();
    
    float desiredRotation = glm::atan(_Input->_Thrust.y, _Input->_Thrust.x) - glm::radians(90.f);
    float rotation = body->GetTransform().q.GetAngle();
    
    if (glm::distance(desiredRotation, rotation) > 3.14f)
    {
        if (desiredRotation > rotation)
        {
            rotation += 3.14f*2.f;
        } else {
            desiredRotation += 3.14f*2.f;
        }
    }
    
    if (glm::length(_Input->_Thrust) < 0.25f || controlLocked)
        desiredRotation = rotation;
    
    float rotateSpeed = 0.030f;
    rotation = glm::mix(rotation, desiredRotation, rotateSpeed);
    
    body->SetTransform(body->GetPosition(), rotation);
    
    glm::vec2 force;
    glm::vec2 rotForce(glm::cos(rotation + glm::radians(90.f)), glm::sin(rotation + glm::radians(90.f)));
    
    if (glm::length(_Input->_Thrust) > 0.f)
        thrustPower *= glm::dot(rotForce, glm::normalize(_Input->_Thrust));
             
    float power = glm::length(_Input->_Thrust)*thrustPower;
    
    if (_Input->_Boost)
    {
        if (_BoostPower > 0.f)
        {
            _BoostPower -= updateMessage.GetDelta();
            force = rotForce * 20.f;
        }
    } else {
        _BoostPower = glm::min(_BoostPower+updateMessage.GetDelta(), 1.5f);
        
        force = glm::vec2(_Input->_Thrust*power);
    }    
    
    if (!controlLocked)
        body->ApplyForceToCenter(b2Vec2(force.x, force.y));
    
    glm::vec2 velocity(body->GetLinearVelocity().x, body->GetLinearVelocity().y);
    
    if (_BarrelCooldown < 0.f)
    {
        float barrelForceStrength = 7.5f;//4000.f;
        if (_Input->_BarrelLeft)
        {
            _BarrelCooldown = barrelCooldown;
            glm::vec2 barrelForce(glm::cos(rotation + glm::radians(180.f)), glm::sin(rotation + glm::radians(180.f)));
            barrelForce *= barrelForceStrength;
            glm::vec2 forwardVelocity = glm::dot(glm::normalize(rotForce), velocity)*rotForce;
            velocity = barrelForce + forwardVelocity;
        } else if (_Input->_BarrelRight)
        {
            _BarrelCooldown = barrelCooldown;
            glm::vec2 barrelForce(glm::cos(rotation), glm::sin(rotation));
            barrelForce *= barrelForceStrength;
            glm::vec2 forwardVelocity = glm::dot(glm::normalize(rotForce), velocity)*rotForce;
            velocity = barrelForce + forwardVelocity;
        }
    } else {
        _BarrelCooldown -= updateMessage.GetDelta();
    }
    
    velocity.x = glm::clamp(velocity.x, -8.f, 8.f) * 0.99f;
    velocity.y = glm::clamp(velocity.y, -8.f, 8.f) * 0.99f;
    body->SetLinearVelocity(b2Vec2(velocity.x, velocity.y));
    
    glm::vec3 position = GetComponentByType<pb::TransformComponent>()->GetPosition();
    
    float maxTilt = 90.f;
    float desiredTilt = glm::clamp(-(desiredRotation - rotation)*60.f, -maxTilt, maxTilt) * glm::clamp(glm::length(velocity)/5.f, 0.f, 1.f);
    _Tilt = glm::mix(_Tilt, desiredTilt, 0.10f); // 0.03f
    
    float barrelRot = 360.f * (1.f-glm::clamp(_BarrelCooldown-(barrelCooldown-barrelLength), 0.f, barrelLength)/barrelLength);
    
    glm::mat4x4 transform;
    transform = glm::rotate(transform, 90.f, glm::vec3(1,0,0));
    transform = glm::rotate(transform, 180.f, glm::vec3(0,1,0));
    transform = glm::rotate(transform, glm::cos(pb::Engine::Instance()->GetGameTime() / 2.f) * 6.f + _Tilt + barrelRot, glm::vec3(0,0,1));
    transform = glm::scale(transform, glm::vec3(0.65, 0.65, 0.65));
    GetComponentByType<pb::ModelComponent>()->SetLocalTransform(transform);
    
    _FiringDelay = glm::max(0.f, _FiringDelay-updateMessage.GetDelta());
    if (_Input->_Firing)
    {
        if (_FiringDelay <= 0.f)
        {
            _FiringDelay += 0.5f;
            
            new Projectile(GetScene(), position, rotation, glm::length(velocity) + 10.f);
            new Projectile(GetScene(), position, rotation, glm::length(velocity) + 10.f);

        }
    }
}

float PlayerShip::GetSpeedPercentage()
{
    b2Body* body = GetComponentByType<pb::PhysicsBody2DComponent>()->GetBody();
    return body->GetLinearVelocity().y / 10.f;
}