#include "Box2D/Box2D.h"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/noise.hpp"

#include "pixelboost/debug/debugVariable.h"
#include "pixelboost/framework/engine.h"
#include "pixelboost/graphics/camera/camera.h"
#include "pixelboost/graphics/particle/particleSystem.h"
#include "pixelboost/graphics/renderer/common/renderer.h"
#include "pixelboost/graphics/renderer/model/modelRenderer.h"
#include "pixelboost/graphics/shader/manager.h"
#include "pixelboost/graphics/shader/shader.h"
#include "pixelboost/logic/component/graphics/model.h"
#include "pixelboost/logic/component/graphics/sprite.h"
#include "pixelboost/logic/component/physics/2d/physicsBody.h"
#include "pixelboost/logic/component/transform/basic.h"
#include "pixelboost/logic/message/physics/collision.h"
#include "pixelboost/logic/system/physics/2d/physics.h"
#include "pixelboost/logic/message/update.h"
#include "pixelboost/logic/scene.h"
#include "pixelboost/maths/matrixHelpers.h"

#include "common/layers.h"
#include "core/game.h"
#include "gameplay/health.h"
#include "player/weapon/laser.h"
#include "player/weapon/missile.h"
#include "player/grapple.h"
#include "player/player.h"
#include "player/projectile.h"
#include "screens/game.h"

DEFINE_DEBUG_FLOAT(g_LightA_X, "LightA.X", 0.f, -1.f, 1.f);
DEFINE_DEBUG_FLOAT(g_LightA_Y, "LightA.Y", 0.f, -1.f, 1.f);
DEFINE_DEBUG_FLOAT(g_LightA_Z, "LightA.Z", 1.f, -1.f, 1.f);
DEFINE_DEBUG_FLOAT(g_LightA_R, "LightA.R", 1.f, 0.f, 1.f);
DEFINE_DEBUG_FLOAT(g_LightA_G, "LightA.G", 1.f, 0.f, 1.f);
DEFINE_DEBUG_FLOAT(g_LightA_B, "LightA.B", 0.9f, 0.f, 1.f);
DEFINE_DEBUG_FLOAT(g_LightA_I, "LightA.I", 1.265f, 0.f, 4.f);
DEFINE_DEBUG_FLOAT(g_LightB_X, "LightB.X", -0.474f, -1.f, 1.f);
DEFINE_DEBUG_FLOAT(g_LightB_Y, "LightB.Y", -0.123f, -1.f, 1.f);
DEFINE_DEBUG_FLOAT(g_LightB_Z, "LightB.Z", -0.884f, -1.f, 1.f);
DEFINE_DEBUG_FLOAT(g_LightB_R, "LightB.R", 0.39f, 0.f, 1.f);
DEFINE_DEBUG_FLOAT(g_LightB_G, "LightB.G", 0.89f, 0.f, 1.f);
DEFINE_DEBUG_FLOAT(g_LightB_B, "LightB.B", 0.9f, 0.f, 1.f);
DEFINE_DEBUG_FLOAT(g_LightB_I, "LightB.I", 1.967f, 0.f, 4.f);
DEFINE_DEBUG_FLOAT(g_LightC_X, "LightC.X", 0.47f, -1.f, 1.f);
DEFINE_DEBUG_FLOAT(g_LightC_Y, "LightC.Y", -0.242f, -1.f, 1.f);
DEFINE_DEBUG_FLOAT(g_LightC_Z, "LightC.Z", 0.513f, -1.f, 1.f);
DEFINE_DEBUG_FLOAT(g_LightC_R, "LightC.R", 0.62f, 0.f, 1.f);
DEFINE_DEBUG_FLOAT(g_LightC_G, "LightC.G", 0.53f, 0.f, 1.f);
DEFINE_DEBUG_FLOAT(g_LightC_B, "LightC.B", 0.67f, 0.f, 1.f);
DEFINE_DEBUG_FLOAT(g_LightC_I, "LightC.I", 0.47f, 0.f, 4.f);

PlayerInput::PlayerInput()
    : _BarrelLeft(false)
    , _BarrelRight(false)
    , _Boost(false)
    , _FiringLeft(false)
    , _FiringRight(false)
{
    
}

PlayerInput::~PlayerInput()
{
    
}

PlayerKeyboardInput::PlayerKeyboardInput()
    : _KeyLeft(false)
    , _KeyRight(false)
    , _KeyUp(false)
    , _KeyDown(false)
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
                _KeyUp = true;
                break;
            case 'a':
                _KeyLeft = true;
                break;
            case 'd':
                _KeyRight = true;
                break;
            case 's':
                _KeyDown = true;
                break;
            case 'o':
                _FiringLeft = true;
                break;
            case 'p':
                _FiringRight = true;
                break;
        }
    }
    
    UpdateThrust();
    
    return false;
}

bool PlayerKeyboardInput::OnKeyUp(pb::KeyboardKey key, pb::ModifierKeys modifier, char character)
{
    if (key == pb::kKeyboardKeyCharacter)
    {
        switch (character)
        {
            case 'w':
                _KeyUp = false;
                break;
            case 'a':
                _KeyLeft = false;
                break;
            case 'd':
                _KeyRight = false;
                break;
            case 's':
                _KeyDown = false;
                break;
            case 'o':
                _FiringLeft = false;
                break;
            case 'p':
                _FiringRight = false;
                break;
        }
    }
    
    UpdateThrust();
    
    return false;
}

void PlayerKeyboardInput::UpdateThrust()
{
    _Thrust.x = (_KeyLeft ? -1.f : 0.f) + (_KeyRight ? 1.f : 0.f);
    _Thrust.y = (_KeyUp ? 1.f : 0.f) + (_KeyDown ? -1.f : 0.f);
    
    if (glm::length(_Thrust) > 0.f)
        _Thrust = glm::normalize(_Thrust);
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
            _FiringLeft = (value > 0);
        if (axis == 1)
            _FiringRight = (value > 0);
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

PlayerShip::PlayerShip(pb::Scene* scene, int playerId, glm::vec2 position)
    : pb::Entity(scene, 0)
    , _BarrelCooldown(0)
    , _Energy(100)
    , _GrappleActive(false)
    , _PlayerId(playerId)
    , _Tilt(0)
    , _ShieldTime(0)
    , _HullTime(0)
{
    pb::TransformComponent* transform = new pb::BasicTransformComponent(this);
    transform->SetPosition(glm::vec3(position, 0.f));
    
    _Ship = new pb::ModelComponent(this,
                                   pb::Engine::Instance()->GetModelRenderer()->GetModel(playerId == 0 ? "ship_01" : "ship_02"),
                                   pb::Engine::Instance()->GetModelRenderer()->GetTexture(playerId == 0 ? "ship_01_DIFF" : "ship_02_DIFF"));
    _Ship->SetLayer(kGraphicLayerPlayer);
    _Ship->SetShader(Game::Instance()->GetLitShader());
    
    pb::PhysicsBody2DComponent* physics = new pb::PhysicsBody2DComponent(this, pb::PhysicsBody2DComponent::kBodyTypeDynamic, pb::PhysicsBody2DComponent::kBodyShapeCircle, glm::vec2(1,1));
    
    physics->GetBody()->SetAngularDamping(0.9f);
    
    _Shield = new pb::ModelComponent(this,
                                     pb::Engine::Instance()->GetModelRenderer()->GetModel("shield"),
                                     pb::Engine::Instance()->GetModelRenderer()->GetTexture("shield_DIFF"));
    _Shield->SetLayer(kGraphicLayerShield);
    _Shield->SetAlphaBlend(true);
    _Shield->SetLocalTransform(glm::scale(glm::translate(glm::mat4x4(), glm::vec3(playerId == 0 ? 0.6 : 0.2,0,0)), glm::vec3(1.2,1.2,1.2)));
    
    _SpriteShield = new pb::SpriteComponent(this, "shield");
    _SpriteShield->SetLayer(kGraphicLayerSpriteShield);
    _SpriteShield->SetLocalTransform(glm::scale(glm::translate(glm::mat4x4(), glm::vec3(playerId == 0 ? 0.2 : -0.2,0,0)), glm::vec3(1.25,1.25,1.25)));
    
    _Input = new PlayerJoystickInput(_PlayerId);
//    _Input = new PlayerKeyboardInput();
    
    _LeftMount.IsLeft = false;
    _LeftMount.Offset = glm::vec3(-0.61, 0, -0.16);
    _LeftMount.Rotation = glm::vec3(0, 0, -15.f);
    
    _RightMount.IsLeft = false;
    _RightMount.Offset = glm::vec3(0.7, 0, -0.16);
    _RightMount.Rotation = glm::vec3(0, 0, 15.f);
    
    _MissileMount.IsLeft = true;
    _MissileMount.Offset = glm::vec3(0.7, 0, -0.16);
    _MissileMount.Rotation = glm::vec3(0,0,15.f);
    
    new HealthComponent(this, kHealthTypePlayer, 50.f, 10.f);
    new LaserComponent(this, _Input, _LeftMount);
    new LaserComponent(this, _Input, _RightMount);
    new MissileComponent(this, _Input, _MissileMount);
    
    _EngineMain = new pb::ParticleComponent(this);
    _EngineLeft = new pb::ParticleComponent(this);
    _EngineRight = new pb::ParticleComponent(this);
    
    SetupEngineParticle(_EngineLeft, glm::vec3(-1.f, -1.f, 0.f), 0.5f);
    SetupEngineParticle(_EngineMain, glm::vec3(0.f, -1.f, 0.f), 1.f);
    SetupEngineParticle(_EngineRight, glm::vec3(1.f, -1.f, 0.f), 0.5f);
    
    RegisterMessageHandler<HullHitMessage>(MessageHandler(this, &PlayerShip::OnHullHit));
    RegisterMessageHandler<ShieldsHitMessage>(MessageHandler(this, &PlayerShip::OnShieldsHit));
    RegisterMessageHandler<pb::PhysicsCollisionMessage>(MessageHandler(this, &PlayerShip::OnCollision));
    RegisterMessageHandler<pb::UpdateMessage>(MessageHandler(this, &PlayerShip::OnUpdate));
}

PlayerShip::~PlayerShip()
{
    UnregisterMessageHandler<HullHitMessage>(MessageHandler(this, &PlayerShip::OnHullHit));
    UnregisterMessageHandler<ShieldsHitMessage>(MessageHandler(this, &PlayerShip::OnShieldsHit));
    UnregisterMessageHandler<pb::PhysicsCollisionMessage>(MessageHandler(this, &PlayerShip::OnCollision));
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

void PlayerShip::OnCollision(const pb::Message& message)
{
    const pb::PhysicsCollisionMessage& collisionMessage = static_cast<const pb::PhysicsCollisionMessage&>(message);
    
    if (collisionMessage.GetOtherComponent()->GetParent()->GetType() == pb::TypeHash("Cog"))
    {
        Game::Instance()->GetGameScreen()->AddScore(10.f);
        collisionMessage.GetOtherComponent()->GetParent()->Destroy();
    }
}

void PlayerShip::OnHullHit(const pb::Message& message)
{
    _HullTime = 1.f;
}

void PlayerShip::OnShieldsHit(const pb::Message& message)
{
    _ShieldTime = 1.f;
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
    float rotation = body->GetAngle();
    
    while (glm::distance(desiredRotation, rotation) >= M_PI)
    {
        if (desiredRotation > rotation)
        {
            rotation += M_PI*2.f;
        } else {
            desiredRotation += M_PI*2.f;
        }
    }
    
    if (glm::length(_Input->_Thrust) < 0.25f || controlLocked)
        desiredRotation = rotation;
    
    float rotateSpeed = 0.090f;
    float prevRotation = rotation;
    rotation = glm::mix(rotation, desiredRotation, rotateSpeed);
    
    body->SetTransform(body->GetPosition(), glm::mod((float)rotation, (float)M_PI*2.f) - M_PI*2.f);
    
    glm::vec2 force;
    glm::vec2 rotForce(glm::cos(rotation + glm::radians(90.f)), glm::sin(rotation + glm::radians(90.f)));
    
    if (glm::length(_Input->_Thrust) > 0.f)
        thrustPower *= glm::dot(rotForce, glm::normalize(_Input->_Thrust));
             
    float power = glm::length(_Input->_Thrust)*thrustPower;
    
    if (_Input->_Boost)
    {
        const float boostCost = 1.f/5.f;
        if (_Energy > boostCost)
        {
            _Energy -= boostCost;
            force = rotForce * 120.f;
        }
    } else {
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
    _Tilt = glm::mix(_Tilt, desiredTilt, 0.30f);
    
    float barrelRot = 360.f * (1.f-glm::clamp(_BarrelCooldown-(barrelCooldown-barrelLength), 0.f, barrelLength)/barrelLength);
    
    glm::mat4x4 transform;
    transform = glm::rotate(transform, 90.f, glm::vec3(1,0,0));
    transform = glm::rotate(transform, 180.f, glm::vec3(0,1,0));
    transform = glm::rotate(transform, _Tilt + barrelRot, glm::vec3(0,0,1));
    GetComponentByType<pb::ModelComponent>()->SetLocalTransform(transform);
    
    _Energy += glm::max(((100.f-_Energy)/25.f),1.f)*updateMessage.GetDelta()*2.f;
    _Energy = glm::min(_Energy, 100.f);
    
    if (glm::length(_Input->_GrappleDirection) > 0.35f && !_GrappleActive)
    {
        _GrappleActive = true;
        
        const pb::Scene::EntityMap& entities = GetScene()->GetEntities();
        
        glm::vec3 nearestEntity;
        for (pb::Scene::EntityMap::const_iterator it = entities.begin(); it != entities.end(); ++it)
        {
            if (it->second == this)
                continue;
            
            if (it->second->GetType() != pb::TypeHash("Asteroid") && it->second->GetType() != pb::TypeHash("HomingMine") && it->second->GetType() != pb::TypeHash("StaticMine") && it->second->GetType() != pb::TypeHash("Turret") && it->second->GetType() != pb::TypeHash("StealthBomber") && it->second->GetType() != pb::TypeHash("Cog"))
                continue;
            
            glm::vec3 entityPosition = it->second->GetComponentByType<pb::TransformComponent>()->GetPosition();
            float dot = glm::dot(glm::normalize(glm::vec2(entityPosition.x - position.x, entityPosition.y - position.y)), glm::normalize(_Input->_GrappleDirection));
            
            if (dot > 0 && glm::degrees(glm::acos(dot)) < 70.f)
            {
                if (glm::distance(position, entityPosition) < 15.f)
                {
                    new Grapple(GetScene(), GetUid(), _Input, it->second->GetUid());
                }
            }
        }
    } else if (glm::length(_Input->_GrappleDirection) < 0.25f) {
        _GrappleActive = false;
    }
    
    ProcessGameBounds();
    ProcessLighting();
    
    pb::ParticleSystem* engineLeft = _EngineLeft->GetSystem();
    pb::ParticleSystem* engineMain = _EngineMain->GetSystem();
    pb::ParticleSystem* engineRight = _EngineRight->GetSystem();
    
    float particleSpeed = glm::length(force);
    engineMain->Definition->StartRotation.Set(glm::vec3(0,0,glm::degrees(rotation)));
    engineMain->Definition->StartSpeed.Set(-0.3f * particleSpeed, -0.4f * particleSpeed);
    engineMain->Definition->Emitter->EmitSpeed = glm::clamp(_Input->_Boost ? 1200.f : glm::length(_Input->_Thrust) * 600.f, 0.f, 1200.f);
    
    engineLeft->Definition->Emitter->EmitSpeed = 0.f;
    engineRight->Definition->Emitter->EmitSpeed = 0.f;
    
    float rotateThruster = glm::clamp(prevRotation - rotation, -2.f, 2.f) * 5.f;
    if (rotateThruster > 0.f)
    {
        engineLeft->Definition->StartRotation.Set(glm::vec3(0,0,glm::degrees(rotation)));
        engineLeft->Definition->StartSpeed.Set(-30.f * rotateThruster, -30.f * rotateThruster);
        engineLeft->Definition->Emitter->EmitSpeed = glm::clamp(glm::abs(rotateThruster) * 1200.f, 0.f, 1200.f);
    } else {
        engineRight->Definition->StartRotation.Set(glm::vec3(0,0,glm::degrees(rotation)));
        engineRight->Definition->StartSpeed.Set(30.f * rotateThruster, 30.f * rotateThruster);
        engineRight->Definition->Emitter->EmitSpeed = glm::clamp(glm::abs(rotateThruster) * 1200.f, 0.f, 1200.f);
    }
    
    _SpriteShield->SetTint(glm::vec4(1.f,1.f,1.f,GetComponentByType<HealthComponent>()->GetShields()/10.f));
    
    _ShieldTime = glm::max(_ShieldTime-updateMessage.GetDelta(), 0.f);
    _Shield->SetTint(glm::vec4(1,1,1,glm::clamp(_ShieldTime*3.f, 0.f, 1.f)));
    
    _HullTime = glm::max(_HullTime-updateMessage.GetDelta(), 0.f);
    _Ship->SetTint(glm::vec4(1, 0.5 + glm::clamp((1.f-_HullTime), 0.f, 0.5f), glm::clamp((1.f-_HullTime), 0.f, 1.f), 1.f));
}

float PlayerShip::GetEnergy()
{
    return _Energy;
}

void PlayerShip::RemoveEnergy(float energy)
{
    _Energy -= energy;
}

float PlayerShip::GetSpeedPercentage()
{
    b2Body* body = GetComponentByType<pb::PhysicsBody2DComponent>()->GetBody();
    return body->GetLinearVelocity().y / 10.f;
}

float PlayerShip::GetTilt()
{
    return _Tilt;
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

void PlayerShip::ProcessLighting()
{
    pb::Shader* shader = pb::Renderer::Instance()->GetShaderManager()->GetShader("/data/shaders/texturedLit.shc");
    pb::ShaderPass* pass = shader->GetTechnique(pb::TypeHash("default"))->GetPass(0);
    pass->Bind();
    
    pb::ShaderProgram* program = pass->GetShaderProgram();
    
    program->SetUniform("_LightDirection[0]", glm::normalize(glm::vec3(g_LightA_X, g_LightA_Y, g_LightA_Z)));
    program->SetUniform("_LightDirection[1]", glm::normalize(glm::vec3(g_LightB_X, g_LightB_Y, g_LightB_Z)));
    program->SetUniform("_LightDirection[2]", glm::normalize(glm::vec3(g_LightC_X, g_LightC_Y, g_LightC_Z)));

    program->SetUniform("_LightColor[0]", glm::vec3(g_LightA_R, g_LightA_G, g_LightA_B)*(float)g_LightA_I);
    program->SetUniform("_LightColor[1]", glm::vec3(g_LightB_R, g_LightB_G, g_LightB_B)*(float)g_LightB_I);
    program->SetUniform("_LightColor[2]", glm::vec3(g_LightC_R, g_LightC_G, g_LightC_B)*(float)g_LightC_I);
}

void PlayerShip::SetupEngineParticle(pb::ParticleComponent* particleComponent, glm::vec3 position, float scale)
{
    particleComponent->SetLayer(kGraphicLayerParticles);
    particleComponent->SetLocalTransform(glm::translate(glm::mat4x4(), position));
    
    pb::ParticleSystemDefinition* engineDefinition = particleComponent->GetSystem()->Definition;
    pb::ParticleDefinitionEmitterCone* emitter = new pb::ParticleDefinitionEmitterCone();
    emitter->EmitSpeed = 0.f;
    emitter->Range = 2.f;
    engineDefinition->RenderSprite = new pb::ParticleSpriteDefinition("engine");
    engineDefinition->StartLife.Set(0.07f, 0.1f);
    pb::ParticleValueCurve1D* scaleValue = new pb::ParticleValueCurve1D();
    scaleValue->Curve.Points.push_back(pb::HermiteCurve2D::Point(glm::vec2(-0.1,0), glm::vec2(0.f,scale), glm::vec2(0.5,0)));
    scaleValue->Curve.Points.push_back(pb::HermiteCurve2D::Point(glm::vec2(-0.2,0), glm::vec2(1.f,0.f), glm::vec2(0.1,0)));
    engineDefinition->ModifierScale = scaleValue;
    engineDefinition->Emitter = emitter;
}
