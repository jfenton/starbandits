
#include "Box2D/Box2D.h"
#include "glm/gtc/matrix_transform.hpp"

#include "pixelboost/framework/engine.h"
#include "pixelboost/logic/component/graphics/sprite.h"
#include "pixelboost/logic/component/physics/2d/physicsBody.h"
#include "pixelboost/logic/component/transform/basic.h"
#include "pixelboost/logic/message/physics/collision.h"
#include "pixelboost/logic/message/update.h"
#include "pixelboost/logic/scene.h"

#include "common/layers.h"
#include "gameplay/damage.h"
#include "gameplay/health.h"
#include "player/player.h"
#include "player/projectile.h"

Projectile::Projectile(pb::Scene* scene, HealthType healthType, BehaviourType behaviour, glm::vec3 position, float rotation, float speed, float damage)
    : pb::Entity(scene, 0)
    , _Behaviour(behaviour)
    , _Life(2.f)
    , _Speed(speed)
{
    new DamageComponent(this, healthType, damage);
    
    pb::BasicTransformComponent* transform = new pb::BasicTransformComponent(this);
    transform->SetRotation(glm::vec3(0,0,glm::degrees(rotation)));
    transform->SetPosition(position);
    
    pb::SpriteComponent* sprite = new pb::SpriteComponent(this, "");
    if (healthType == kHealthTypeEnemy)
    {
        sprite->SetSprite("laser_red");
    } else {
        if (behaviour == kBehaviourTypeHoming)
        {
            sprite->SetSprite("missile_01");
        } else {
            sprite->SetSprite("laser_green");
        }
    }
    glm::mat4x4 spriteTransform;
    spriteTransform = glm::translate(spriteTransform, glm::vec3(0,1,0));
    spriteTransform = glm::rotate(spriteTransform, -90.f, glm::vec3(0,0,1));
    
    if (healthType == kHealthTypeEnemy)
    {
        spriteTransform = glm::scale(spriteTransform, glm::vec3(1.5,1.5,1));
    }
    
    sprite->SetLocalTransform(spriteTransform);
    sprite->SetLayer(kGraphicLayerProjectiles);
    
    pb::PhysicsBody2DComponent* physics = new pb::PhysicsBody2DComponent(this, pb::PhysicsBody2DComponent::kBodyTypeDynamic, pb::PhysicsBody2DComponent::kBodyShapeRect, glm::vec2(0.08,0.4));
    physics->SetSensor(true);
    physics->GetBody()->SetBullet(true);
    
    if (_Behaviour == kBehaviourTypeLaser)
    {
        physics->GetBody()->SetLinearVelocity(b2Vec2(cos(rotation+glm::radians(90.f))*speed, sin(rotation+glm::radians(90.f))*speed));
    }
    
    if (_Behaviour == kBehaviourTypeHoming)
    {
        _Life = 15.f;
        SetTarget();
    }
    
    RegisterMessageHandler<pb::PhysicsCollisionMessage>(MessageHandler(this, &Projectile::OnCollision));
    RegisterMessageHandler<pb::UpdateMessage>(MessageHandler(this, &Projectile::OnUpdate));
}

Projectile::~Projectile()
{
    UnregisterMessageHandler<pb::PhysicsCollisionMessage>(MessageHandler(this, &Projectile::OnCollision));
    UnregisterMessageHandler<pb::UpdateMessage>(MessageHandler(this, &Projectile::OnUpdate));
}

pb::Uid Projectile::GetType() const
{
    return GetStaticType();
}

pb::Uid Projectile::GetStaticType()
{
    return pb::TypeHash("Projectile");
}

void Projectile::OnCollision(const pb::Message& message)
{
    const pb::PhysicsCollisionMessage& collisionMessage = static_cast<const pb::PhysicsCollisionMessage&>(message);
    
    HealthComponent* health = collisionMessage.GetOtherComponent()->GetParent()->GetComponentByType<HealthComponent>();
    
    pb::Uid type = collisionMessage.GetOtherComponent()->GetParent()->GetType();
    if (type == GetStaticType() || type == pb::TypeHash("Explosion"))
        return;
    
    if (!health || GetComponentByType<DamageComponent>()->GetHealthType() != health->GetHealthType())
        Destroy();
}

void Projectile::OnUpdate(const pb::Message& message)
{
    const pb::UpdateMessage& updateMessage = static_cast<const pb::UpdateMessage&>(message);
    
    _Life -= updateMessage.GetDelta();
    
    if (_Life <= 0.f)
        Destroy();
    
    UpdateBehaviour();
}

void Projectile::UpdateBehaviour()
{
    switch (_Behaviour)
    {
        case kBehaviourTypeLaser:
            break;
            
        case kBehaviourTypeBeam:
            break;
            
        case kBehaviourTypeHoming:
        {
            b2Body* body = GetComponentByType<pb::PhysicsBody2DComponent>()->GetBody();
            
            glm::vec2 desiredDirection;
            
            pb::Entity* target = GetScene()->GetEntityById(_Target);
            pb::TransformComponent* targetTransform = target ? target->GetComponentByType<pb::TransformComponent>() : 0;
            
            if (!target || !targetTransform)
            {
                SetTarget();
                body->ApplyForceToCenter(b2Vec2(glm::cos(body->GetAngle()), glm::sin(body->GetAngle())));
                return;
            }
            
            desiredDirection = glm::vec2(targetTransform->GetPosition().x, targetTransform->GetPosition().y) - glm::vec2(body->GetPosition().x, body->GetPosition().y);
            
            desiredDirection = glm::normalize(desiredDirection);
            
            if (glm::length(desiredDirection) > 0.f)
            {
                float desiredRotation = glm::atan(desiredDirection.y, desiredDirection.x) - glm::radians(90.f);
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
                
                float rotateSpeed = 0.090f;
                rotation = glm::mix(rotation, desiredRotation, rotateSpeed);
                
                body->SetTransform(body->GetPosition(), rotation);
                body->ApplyForceToCenter(b2Vec2(desiredDirection.x*_Speed, desiredDirection.y*_Speed));
                body->SetLinearDamping(0.8f);
            }
            
            break;
        }
    }
}

void Projectile::SetTarget()
{
    glm::vec3 position = GetComponentByType<pb::TransformComponent>()->GetPosition();
    float rotation = GetComponentByType<pb::PhysicsBody2DComponent>()->GetBody()->GetAngle();
    float closestTarget = 10000.f;
    
    for (pb::Scene::EntityMap::const_iterator it = GetScene()->GetEntities().begin(); it != GetScene()->GetEntities().end(); ++it)
    {
        if (it->second->GetType() == GetStaticType() || it->second->GetType() == pb::TypeHash("Explosion"))
            continue;
        
        pb::TransformComponent* transform = it->second->GetComponentByType<pb::TransformComponent>();
        
        HealthComponent* health = it->second->GetComponentByType<HealthComponent>();
        
        if (!transform || !health)
            continue;
        
        if (it->second->GetType() != pb::TypeHash("Asteroid") && it->second->GetType() != pb::TypeHash("HomingMine") && it->second->GetType() != pb::TypeHash("StaticMine") && it->second->GetType() != pb::TypeHash("Turret"))
            continue;
        
        if (health->GetHealthType() == GetComponentByType<DamageComponent>()->GetHealthType())
            continue;
        
        glm::vec3 entityPosition = it->second->GetComponentByType<pb::TransformComponent>()->GetPosition();
        glm::vec2 entityDirection = glm::normalize(glm::vec2(entityPosition.x - position.x, entityPosition.y - position.y));
        glm::vec2 currentDirection(glm::cos(rotation+glm::radians(90.f)), glm::sin(rotation+glm::radians(90.f)));
        float dot = glm::dot(entityDirection, currentDirection);
        
        float distance = glm::distance(position, entityPosition);
                
        if (dot > 0 && glm::degrees(glm::acos(dot)) < 45.f && distance < closestTarget)
        {
            closestTarget = distance;
            _Target = it->first;
        }
    }
}
