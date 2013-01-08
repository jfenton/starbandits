#include "Box2D/Box2D.h"
#include "glm/gtc/matrix_transform.hpp"

#include "pixelboost/framework/engine.h"
#include "pixelboost/graphics/renderer/model/modelRenderer.h"
#include "pixelboost/logic/component/graphics/model.h"
#include "pixelboost/logic/component/physics/2d/physicsBody.h"
#include "pixelboost/logic/component/transform/basic.h"
#include "pixelboost/logic/message/update.h"

#include "player/projectile.h"
#include "player/ship.h"

PlayerShip::PlayerShip(pb::Scene* scene)
    : pb::Entity(scene, 0)
    , _ThrustBackward(0)
    , _ThrustForward(0)
    , _ThrustLeft(0)
    , _ThrustRight(0)
{
    new pb::BasicTransformComponent(this);
    
    pb::ModelComponent* model = new pb::ModelComponent(this,
                           pb::Engine::Instance()->GetModelRenderer()->GetModel("ship"),
                           pb::Engine::Instance()->GetModelRenderer()->GetTexture("ship"));
    model->SetLocalTransform(glm::scale(glm::mat4x4(), glm::vec3(0.5, 0.5, 0.5)));
    model->SetLayer(1);
    
    pb::PhysicsBody2DComponent* physics = new pb::PhysicsBody2DComponent(this, pb::PhysicsBody2DComponent::kBodyTypeDynamic, pb::PhysicsBody2DComponent::kBodyShapeRect, glm::vec2(2,5));
    
    pb::Engine::Instance()->GetKeyboardManager()->AddHandler(this);
    
    RegisterMessageHandler<pb::UpdateMessage>(MessageHandler(this, &PlayerShip::OnUpdate));
}

PlayerShip::~PlayerShip()
{
    UnregisterMessageHandler<pb::UpdateMessage>(MessageHandler(this, &PlayerShip::OnUpdate));
    
    pb::Engine::Instance()->GetKeyboardManager()->RemoveHandler(this);
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
    const pb::UpdateMessage& updateMessage = static_cast<const pb::UpdateMessage&>(message);
    float thrustPowerX = 6000.f * updateMessage.GetDelta();
    float thrustPowerY = 5000.f * updateMessage.GetDelta();
    
    b2Body* body = GetComponentByType<pb::PhysicsBody2DComponent>()->GetBody();
    body->ApplyForceToCenter(b2Vec2(_ThrustLeft * -thrustPowerX + _ThrustRight * thrustPowerX, _ThrustForward * thrustPowerY + _ThrustBackward * 0.15f * -thrustPowerY));
    
    b2Vec2 velocity = body->GetLinearVelocity();
    velocity.x = glm::clamp(velocity.x, -8.f, 8.f) * 0.97f;
    velocity.y = glm::clamp(velocity.y, 1.f, 10.f) * 0.996f;
    body->SetLinearVelocity(velocity);
    
    glm::mat4x4 transform;
    transform = glm::rotate(transform, 90.f, glm::vec3(1,0,0));
    transform = glm::rotate(transform, 180.f, glm::vec3(0,1,0));
    transform = glm::rotate(transform, 10.f * velocity.x, glm::vec3(0,0,1));
    transform = glm::scale(transform, glm::vec3(0.3, 0.3, 0.3));
    GetComponentByType<pb::ModelComponent>()->SetLocalTransform(transform);
    
    if (_Firing)
    {
        _FiringDelay -= updateMessage.GetDelta();
        
        if (_FiringDelay <= 0.f)
        {
            _FiringDelay += 0.5f;
            
            new Projectile(GetScene(), GetComponentByType<pb::TransformComponent>()->GetPosition() - glm::vec3(0.8, 0, 0), velocity.y + 10.f);
            new Projectile(GetScene(), GetComponentByType<pb::TransformComponent>()->GetPosition() + glm::vec3(2.3, 0, 0), velocity.y + 10.f);

        }
    }
}

bool PlayerShip::OnKeyDown(pb::KeyboardKey key, char character)
{
    if (key == pb::kKeyboardKeyCharacter)
    {
        switch (character)
        {
            case 'w':
                _ThrustForward = 1.f;
                break;
            case 'a':
                _ThrustLeft = 1.f;
                break;
            case 'd':
                _ThrustRight = 1.f;
                break;
            case 's':
                _ThrustBackward = 1.f;
                break;
        }
    }
    
    if (key == pb::kKeyboardKeySpace)
    {
        _Firing = true;
        _FiringDelay = 0.f;
    }
    
    return false;
}


bool PlayerShip::OnKeyUp(pb::KeyboardKey key, char character)
{
    if (key == pb::kKeyboardKeyCharacter)
    {
        switch (character)
        {
            case 'w':
                _ThrustForward = 0.f;
                break;
            case 'a':
                _ThrustLeft = 0.f;
                break;
            case 'd':
                _ThrustRight = 0.f;
                break;
            case 's':
                _ThrustBackward = 0.f;
                break;
        }
    }
    
    if (key == pb::kKeyboardKeySpace)
    {
        _Firing = false;
    }
    
    return false;
}
