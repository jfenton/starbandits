#include "Box2D/Box2D.h"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/noise.hpp"

#include "pixelboost/framework/engine.h"
#include "pixelboost/graphics/renderer/model/modelRenderer.h"
#include "pixelboost/logic/component/graphics/model.h"
#include "pixelboost/logic/component/physics/2d/physicsBody.h"
#include "pixelboost/logic/component/transform/basic.h"
#include "pixelboost/logic/system/physics/2d/physics.h"
#include "pixelboost/logic/message/update.h"
#include "pixelboost/logic/scene.h"

#include "common/layers.h"
#include "core/game.h"
#include "gameplay/health.h"
#include "player/grappleComponent.h"
#include "player/grapple.h"
#include "player/player.h"
#include "player/projectile.h"
#include "screens/game.h"

PlayerInput::PlayerInput()
    : _BarrelLeft(false)
    , _BarrelRight(false)
    , _Boost(false)
    , _Firing(false)
    , _Grapple(false)
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

bool PlayerKeyboardInput::OnKeyDown(pb::KeyboardKey key, pb::ModifierKeys modifier, char character)
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


bool PlayerKeyboardInput::OnKeyUp(pb::KeyboardKey key, pb::ModifierKeys modifier, char character)
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
                _GrappleDirection.x = value;
            } else {
                _GrappleDirection.x = 0.f;
            }
        } else {
            if (glm::abs(value) > 0.25f)
            {
                _GrappleDirection.y = -value;
            } else {
                _GrappleDirection.y = 0.f;
            }
        }
    } else if (stick == 2)
    {
        if (axis == 0)
            _Grapple = (value > 0);
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

PlayerShip::PlayerShip(pb::Scene* scene, int playerId)
    : pb::Entity(scene, 0)
    , _BarrelCooldown(0)
    , _BoostPower(1.5)
    , _FiringDelay(0)
    , _GrappleId(0)
    , _GrappleJointA(0)
    , _GrappleJointB(0)
    , _PlayerId(playerId)
    , _Tilt(0)
{
    new pb::BasicTransformComponent(this);
    
    pb::ModelComponent* model = new pb::ModelComponent(this,
                           pb::Engine::Instance()->GetModelRenderer()->GetModel("ship"),
                           pb::Engine::Instance()->GetModelRenderer()->GetTexture("ship"));
    model->SetLayer(kGraphicLayerPlayer);
    
    pb::PhysicsBody2DComponent* physics = new pb::PhysicsBody2DComponent(this, pb::PhysicsBody2DComponent::kBodyTypeDynamic, pb::PhysicsBody2DComponent::kBodyShapeCircle, glm::vec2(1,1));
    
    physics->GetBody()->SetAngularDamping(0.5f);
    
    _Input = new PlayerJoystickInput(_PlayerId);
    
    new HealthComponent(this, kHealthTypePlayer, 50.f, 100.f);
    
    RegisterMessageHandler<pb::UpdateMessage>(MessageHandler(this, &PlayerShip::OnUpdate));
}

PlayerShip::~PlayerShip()
{
    UnregisterMessageHandler<pb::UpdateMessage>(MessageHandler(this, &PlayerShip::OnUpdate));
    
    //delete _Input;
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
    float barrelControlLock = 0.25f;
    float barrelLength = 0.5f;
    float barrelCooldown = 1.f;
    float maxSpeed = 48.f;
    float enginePower = 1800.f;
    
    bool controlLocked = _BarrelCooldown-(barrelCooldown-barrelControlLock) > 0.f;
    
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
    
    float rotateSpeed = 0.090f;
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
            force = rotForce * 120.f;
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
        float barrelForceStrength = 20.f;//4000.f;
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
    
    velocity.x = glm::clamp(velocity.x, -maxSpeed, maxSpeed) * 0.97f;
    velocity.y = glm::clamp(velocity.y, -maxSpeed, maxSpeed) * 0.97f;
    body->SetLinearVelocity(b2Vec2(velocity.x, velocity.y));
    
    glm::vec3 position = GetComponentByType<pb::TransformComponent>()->GetPosition();
    
    float maxTilt = 90.f;
    float desiredTilt = glm::clamp(-(desiredRotation - rotation)*60.f, -maxTilt, maxTilt) * glm::clamp(glm::length(velocity)/5.f, 0.f, 1.f);
    _Tilt = glm::mix(_Tilt, desiredTilt, 0.30f); // 0.03f
    
    float barrelRot = 360.f * (1.f-glm::clamp(_BarrelCooldown-(barrelCooldown-barrelLength), 0.f, barrelLength)/barrelLength);
    
    glm::mat4x4 transform;
    transform = glm::rotate(transform, 90.f, glm::vec3(1,0,0));
    transform = glm::rotate(transform, 180.f, glm::vec3(0,1,0));
    transform = glm::rotate(transform, glm::cos(pb::Engine::Instance()->GetGameTime() / 2.f) * 6.f + _Tilt + barrelRot, glm::vec3(0,0,1));
    GetComponentByType<pb::ModelComponent>()->SetLocalTransform(transform);
    
    _FiringDelay = glm::max(0.f, _FiringDelay-updateMessage.GetDelta());
    if (_Input->_Firing)
    {
        if (_FiringDelay <= 0.f)
        {
            _FiringDelay += 0.2f;
            
            new Projectile(GetScene(), kHealthTypePlayer, position, rotation, glm::length(velocity) + 20.f);

        }
    }
    
    if (glm::length(_Input->_GrappleDirection) > 0.25f)
    {
        if (!_GrappleId)
        {
            const pb::Scene::EntityMap& entities = GetScene()->GetEntities();
            
            float grappleDir = glm::atan(_Input->_GrappleDirection.y, _Input->_GrappleDirection.x) - glm::radians(90.f);
            
            float shortestDistance = 10000.f;
            glm::vec3 nearestEntity;
            for (pb::Scene::EntityMap::const_iterator it = entities.begin(); it != entities.end(); ++it)
            {
                if (it->second == this)
                    continue;
                
                if (it->second->GetType() != pb::TypeHash("Asteroid") && it->second->GetType() != pb::TypeHash("HomingMine") && it->second->GetType() != pb::TypeHash("StaticMine"))
                    continue;
                
                glm::vec3 entityPosition = it->second->GetComponentByType<pb::TransformComponent>()->GetPosition();
                float dot = glm::dot(glm::normalize(glm::vec2(entityPosition.x - position.x, entityPosition.y - position.y)), glm::normalize(_Input->_GrappleDirection));
                float length = glm::length(position - (entityPosition + (dot*(entityPosition-position))));
                
                if (dot > 0 && glm::degrees(glm::acos(dot)) < 45.f && length < shortestDistance)
                {
                    nearestEntity = entityPosition;
                    shortestDistance = length;
                }
            }
            
            if (shortestDistance < 10000.f)
            {
                float direction = glm::atan(position.y - nearestEntity.y, position.x - nearestEntity.x) + glm::radians(90.f);
                
                pb::Entity* entity = new Grapple(GetScene(), GetUid(), position, direction, glm::length(velocity) + 20.f);

                _GrappleId = entity->GetUid();
            }
        } else {
            pb::Entity* grappleObject = GetScene()->GetEntityById(_GrappleObject);
            
            if (grappleObject)
            {
                b2Body* objectBody = grappleObject->GetComponentByType<pb::PhysicsBody2DComponent>()->GetBody();
                b2Body* playerBody = GetComponentByType<pb::PhysicsBody2DComponent>()->GetBody();
                
                glm::vec2 direction = glm::vec2(objectBody->GetPosition().x-playerBody->GetPosition().x, objectBody->GetPosition().y-playerBody->GetPosition().y);
                float distance = glm::length(direction);
                float objectForceScale = 0.001f;// * (objectBody->GetMass() / 200.f);
                float playerForceScale = 0.01f;

                objectBody->ApplyLinearImpulse(-b2Vec2(direction.x*objectBody->GetMass()*objectForceScale, direction.y*objectBody->GetMass()*objectForceScale), objectBody->GetPosition());
                playerBody->ApplyLinearImpulse(b2Vec2(direction.x*playerBody->GetMass()*playerForceScale, direction.y*playerBody->GetMass()*playerForceScale), playerBody->GetPosition());
            }
        }
    } else {
        if (_GrappleId)
        {
            pb::Entity::ComponentList grappleComponents = GetComponentsByType<GrappleComponent>();
            
            for (pb::Entity::ComponentList::iterator it = grappleComponents.begin(); it != grappleComponents.end(); it++)
            {
                DestroyComponent(*it);
            }
            
            pb::Entity* grapple = GetScene()->GetEntityById(_GrappleId);
            if (grapple)
            {
                grapple->Destroy();
            }
            _GrappleId = 0;
            
            b2World* world = GetScene()->GetSystemByType<pb::PhysicsSystem2D>()->GetPhysicsWorld();
            
            if (_GrappleJointA)
            {
                world->DestroyJoint(_GrappleJointA);
                _GrappleJointA = 0;
            }
            
            if (_GrappleJointB)
            {
                world->DestroyJoint(_GrappleJointB);
                _GrappleJointB = 0;
            }
        }
    }
    
    ProcessGameBounds();
}

float PlayerShip::GetSpeedPercentage()
{
    b2Body* body = GetComponentByType<pb::PhysicsBody2DComponent>()->GetBody();
    return body->GetLinearVelocity().y / 10.f;
}

void PlayerShip::SetGrappleJoints(b2Joint* jointA, b2Joint* jointB)
{
    _GrappleJointA = jointA;
    _GrappleJointB = jointB;
}

void PlayerShip::SetGrappleObject(pb::Uid grappleObject)
{
    _GrappleObject = grappleObject;
}

void PlayerShip::ProcessGameBounds()
{
    b2Body* body = GetComponentByType<pb::PhysicsBody2DComponent>()->GetBody();
    glm::vec3 position = GetComponentByType<pb::TransformComponent>()->GetPosition();
    
    glm::vec4 bounds = Game::Instance()->GetGameScreen()->GetArenaBounds();
    float forceStrength = 3.f;
    float maxForce = 1.5f;
    
    if (position.x > bounds[0]+bounds[2])
    {
        if (body->GetLinearVelocity().x > -maxForce)
        {
            body->ApplyForceToCenter(b2Vec2(-glm::pow(position.x-(bounds[0]+bounds[2]), forceStrength), 0));
        }
    }
    
    if (position.x < bounds[0]-bounds[2])
    {
        if (body->GetLinearVelocity().x < maxForce)
        {
            body->ApplyForceToCenter(b2Vec2(glm::pow(glm::abs(position.x-(bounds[0]-bounds[2])), forceStrength), 0));
        }
    }
    
    if (position.y > bounds[1]+bounds[3])
    {
        if (body->GetLinearVelocity().y > -maxForce)
        {
            body->ApplyForceToCenter(b2Vec2(0, -glm::pow(position.y-(bounds[1]+bounds[3]), forceStrength)));
        }
    }
    
    if (position.y < bounds[1]-bounds[3])
    {
        if (body->GetLinearVelocity().y < (maxForce*4.f))
        {
            body->ApplyForceToCenter(b2Vec2(0, glm::pow(glm::abs(position.y-(bounds[1]-bounds[3])), forceStrength)));
        }
    }
}
