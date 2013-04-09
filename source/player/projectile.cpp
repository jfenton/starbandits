
#include "Box2D/Box2D.h"
#include "glm/gtc/matrix_transform.hpp"

#include "pixelboost/framework/engine.h"
#include "pixelboost/graphics/particle/particleSystem.h"
#include "pixelboost/logic/component/graphics/sprite.h"
#include "pixelboost/logic/component/physics/2d/physicsBody.h"
#include "pixelboost/logic/component/transform.h"
#include "pixelboost/logic/message/physics/collision.h"
#include "pixelboost/logic/message/update.h"
#include "pixelboost/logic/scene.h"

#include "common/layers.h"
#include "gameplay/damage.h"
#include "gameplay/health.h"
#include "player/player.h"
#include "player/projectile.h"

PB_DEFINE_ENTITY(Projectile)

Projectile::Projectile(pb::Scene* scene, pb::Entity* entity, pb::DbEntity* creationEntity)
    : pb::Entity(scene, entity, creationEntity)
    , _Life(2.f)
    , _Target(0)
{
    
}

void Projectile::Initialise(HealthType healthType, ProjectileType type, glm::vec3 position, float rotation, float speed, float damage)
{
    _Speed = speed;
    _Type = type;
    
    CreateComponent<DamageComponent>()->Initialise(healthType, damage);
    
    auto transform = CreateComponent<pb::TransformComponent>();
    transform->SetRotation(glm::vec3(0,0,glm::degrees(rotation)));
    transform->SetPosition(position);
    
    if (_Type == kProjectileTypeHoming)
    {
        /*
        pb::ParticleComponent* particleComponent = new pb::ParticleComponent(this);
        particleComponent->SetLayer(kGraphicLayerProjectiles);
        
        pb::ParticleSystemDefinition* engineDefinition = particleComponent->GetSystem()->Definition;
        pb::ParticleDefinitionEmitterCone* emitter = new pb::ParticleDefinitionEmitterCone();
        emitter->EmitCount = -1.f;
        emitter->EmitSpeed = 150.f;
        emitter->Range = 180.f;
        if (healthType == kHealthTypeEnemy) {
            engineDefinition->RenderSprite = new pb::ParticleSpriteDefinition("missile_04");
        } else {
            engineDefinition->RenderSprite = new pb::ParticleSpriteDefinition("missile_03");
        }
        engineDefinition->StartLife.Set(0.05f, 0.1f);
        pb::ParticleValueCurve1D* scaleValue = new pb::ParticleValueCurve1D();
        scaleValue->Curve.Points.push_back(pb::HermiteCurve2D::Point(glm::vec2(-0.1,0), glm::vec2(0.f,1.5f), glm::vec2(0.5,0)));
        scaleValue->Curve.Points.push_back(pb::HermiteCurve2D::Point(glm::vec2(-0.2,0), glm::vec2(1.f,0.4f), glm::vec2(0.1,0)));
        engineDefinition->ModifierScale = scaleValue;
        engineDefinition->Emitter = emitter;
        */
    } else {
        auto sprite = CreateComponent<pb::SpriteComponent>();
        if (healthType == kHealthTypeEnemy)
        {
            sprite->SetSprite("/spritesheets/game", "laser_red");
        } else {
            sprite->SetSprite("/spritesheets/game", "laser_green");
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
    }
    
    auto physics = CreateComponent<pb::PhysicsBody2DComponent>();
    physics->Initialise(pb::PhysicsBody2DComponent::kBodyTypeDynamic, pb::PhysicsBody2DComponent::kBodyShapeRect, glm::vec2(0.08,0.4));
    physics->SetSensor(true);
    physics->GetBody()->SetBullet(true);
    
    if (_Type == kProjectileTypeLaser)
    {
        physics->GetBody()->SetLinearVelocity(b2Vec2(cos(rotation+glm::radians(90.f))*speed, sin(rotation+glm::radians(90.f))*speed));
    }
    
    if (_Type == kProjectileTypeHoming)
    {
        _Life = 10.f;
        SetTarget();
    }
    
    RegisterMessageHandler<pb::PhysicsCollisionMessage>(pb::MessageHandler(this, &Projectile::OnCollision));
    RegisterMessageHandler<pb::UpdateMessage>(pb::MessageHandler(this, &Projectile::OnUpdate));
}

Projectile::~Projectile()
{
    UnregisterMessageHandler<pb::PhysicsCollisionMessage>(pb::MessageHandler(this, &Projectile::OnCollision));
    UnregisterMessageHandler<pb::UpdateMessage>(pb::MessageHandler(this, &Projectile::OnUpdate));
}

void Projectile::OnCollision(const pb::Message& message)
{
    const pb::PhysicsCollisionMessage& collisionMessage = static_cast<const pb::PhysicsCollisionMessage&>(message);
    
    HealthComponent* health = collisionMessage.GetOtherComponent()->GetEntity()->GetComponent<HealthComponent>();
    
    pb::Uid type = collisionMessage.GetOtherComponent()->GetEntity()->GetType();
    
    switch (_Type)
    {
        case kProjectileTypeLaser:
        {
            if (type == GetStaticType() || type == pb::TypeHash("Explosion") || type == pb::TypeHash("Cog"))
                return;
            break;
        }
        case kProjectileTypeHoming:
        {
            if (type == GetStaticType() && static_cast<Projectile*>(collisionMessage.GetOtherComponent()->GetEntity())->_Type == kProjectileTypeHoming)
                return;
            break;
        }
        case kProjectileTypeBeam:
        {
            break;
        }
    }
    
    if (!health || GetComponent<DamageComponent>()->GetHealthType() != health->GetHealthType())
        Destroy();
}

void Projectile::OnUpdate(const pb::Message& message)
{
    const pb::UpdateMessage& updateMessage = static_cast<const pb::UpdateMessage&>(message);
    
    _Life -= updateMessage.GetGameDelta();
    
    if (_Life <= 0.f)
        Destroy();
    
    UpdateProjectile();
}

void Projectile::UpdateProjectile()
{
    switch (_Type)
    {
        case kProjectileTypeLaser:
            break;
            
        case kProjectileTypeBeam:
            break;
            
        case kProjectileTypeHoming:
        {
            b2Body* body = GetComponent<pb::PhysicsBody2DComponent>()->GetBody();
            
            glm::vec2 desiredDirection;
            
            pb::Entity* target = GetScene()->GetEntityById(_Target);
            pb::TransformComponent* targetTransform = target ? target->GetComponent<pb::TransformComponent>() : 0;
            
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
    glm::vec3 position = GetComponent<pb::TransformComponent>()->GetPosition();
    float rotation = GetComponent<pb::PhysicsBody2DComponent>()->GetBody()->GetAngle();
    float closestTarget = 10000.f;
    
    for (pb::Scene::EntityMap::const_iterator it = GetScene()->GetEntities().begin(); it != GetScene()->GetEntities().end(); ++it)
    {
        if (it->second->GetType() == GetStaticType() || it->second->GetType() == pb::TypeHash("Explosion"))
            continue;
        
        pb::TransformComponent* transform = it->second->GetComponent<pb::TransformComponent>();
        
        HealthComponent* health = it->second->GetComponent<HealthComponent>();
        
        if (!transform || !health)
            continue;
        
        if (health->GetHealthType() == GetComponent<DamageComponent>()->GetHealthType())
            continue;
        
        glm::vec3 entityPosition = it->second->GetComponent<pb::TransformComponent>()->GetPosition();
        glm::vec2 entityDirection = glm::normalize(glm::vec2(entityPosition.x - position.x, entityPosition.y - position.y));
        glm::vec2 currentDirection(glm::cos(rotation+glm::radians(90.f)), glm::sin(rotation+glm::radians(90.f)));
        float dot = glm::dot(entityDirection, currentDirection);
        
        float distance = glm::distance(position, entityPosition);
                
        if (dot > 0 && glm::degrees(glm::acos(dot)) < 30.f && distance < closestTarget)
        {
            closestTarget = distance;
            _Target = it->first;
        }
    }
}
