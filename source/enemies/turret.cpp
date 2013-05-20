#include "Box2D/Box2D.h"
#include "glm/gtc/matrix_transform.hpp"

#include "pixelboost/framework/engine.h"
#include "pixelboost/graphics/renderer/model/modelRenderer.h"
#include "pixelboost/logic/component/graphics/model.h"
#include "pixelboost/logic/component/physics/2d/physicsBody.h"
#include "pixelboost/logic/component/transform.h"
#include "pixelboost/logic/message/update.h"
#include "pixelboost/logic/scene.h"

#include "common/layers.h"
#include "core/game.h"
#include "enemies/turret.h"
#include "gameplay/explosion.h"
#include "gameplay/health.h"
#include "player/player.h"
#include "player/projectile.h"

PB_DEFINE_ENTITY(Turret)

Turret::Turret(pb::Scene* scene, pb::Entity* parent, pb::DbEntity* creationEntity)
    : pb::Entity(scene, parent, creationEntity)
    , _Type(kProjectileTypeBeam)
    , _ShootTimer(0)
{
    
}

Turret* Turret::Initialise(glm::vec2 position, ProjectileType type)
{
    _Type = type;
    
    auto transform = CreateComponent<pb::TransformComponent>();
    transform->SetPosition(glm::vec3(position, 0));
    
    glm::mat4x4 localTransform;
    localTransform = glm::rotate(localTransform, 90.f, glm::vec3(1,0,0));
    localTransform = glm::rotate(localTransform, 180.f, glm::vec3(0,1,0));
    
    auto turret = CreateComponent<pb::ModelComponent>();
    turret->SetModel("/models/turret");
    turret->SetMaterial(type == kProjectileTypeHoming ? "/materials/turret_homing" : "/materials/turret_laser");
    turret->SetLocalTransform(localTransform);
    turret->SetLayer(kGraphicLayerEnemies);
    turret->SetShader(Game::Instance()->GetLitShader());
    
    /*
    pb::ModelComponent* weapon = new pb::ModelComponent(this,
                                                        pb::ModelRenderer::Instance()->GetModel(type == kProjectileTypeHoming ? "turret_homing" : "turret_laser"),
                                                        pb::ModelRenderer::Instance()->GetTexture("turret_weapons"));
    weapon->SetLocalTransform(localTransform);
    weapon->SetLayer(kGraphicLayerEnemies);
    weapon->SetShader(Game::Instance()->GetLitShader());
    */
    
    auto physics = CreateComponent<pb::PhysicsBody2DComponent>();
    physics->Initialise(pb::PhysicsBody2DComponent::kBodyTypeDynamic, pb::PhysicsBody2DComponent::kBodyShapeCircle, glm::vec2(1.f, 1.f));
    physics->GetBody()->GetFixtureList()[0].SetDensity(20.f);
    physics->GetBody()->ResetMassData();
    
    CreateComponent<HealthComponent>()->Initialise(kHealthTypeEnemy, 75.f, 0.f);
    
    RegisterMessageHandler<pb::UpdateMessage>(pb::MessageHandler(this, &Turret::OnUpdate));
    RegisterMessageHandler<HealthDepletedMessage>(pb::MessageHandler(this, &Turret::OnHealthDepleted));
    
    return this;
}

Turret::~Turret()
{
    UnregisterMessageHandler<pb::UpdateMessage>(pb::MessageHandler(this, &Turret::OnUpdate));
    UnregisterMessageHandler<HealthDepletedMessage>(pb::MessageHandler(this, &Turret::OnHealthDepleted));
}

void Turret::OnUpdate(const pb::Message& message)
{
    const pb::UpdateMessage& updateMessage = static_cast<const pb::UpdateMessage&>(message);
    
    pb::TransformComponent* transform = GetComponent<pb::TransformComponent>();
    glm::vec3 position = transform->GetPosition();
    
    b2Body* body = GetComponent<pb::PhysicsBody2DComponent>()->GetBody();
    
    _ShootTimer -= updateMessage.GetGameDelta();
    
    glm::vec3 direction = GetNearestPlayer() - position;
    
    direction = glm::normalize(direction);
    
    if (glm::length(direction) > 0.f)
    {
        float desiredRotation = glm::atan(direction.y, direction.x) - glm::radians(90.f);
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
    }
    
    if (_ShootTimer <= 0.f)
    {
        GetScene()->CreateEntity<Projectile>(0,0)->Initialise(kHealthTypeEnemy, _Type, position, body->GetAngle(), _Type == kProjectileTypeHoming ? 2.5f : 10.f, 20.f);
        _ShootTimer = _Type == kProjectileTypeHoming ? 2.5f : 1.f;
    }
}

void Turret::OnHealthDepleted(const pb::Message& message)
{
    glm::vec3 position = GetComponent<pb::TransformComponent>()->GetPosition();
    
    Destroy();
    GetScene()->CreateEntity<Explosion>(0,0)->Initialise(glm::vec2(position.x, position.y), 3.f);
}

glm::vec3 Turret::GetNearestPlayer()
{
    for (pb::Scene::EntityMap::const_iterator it = GetScene()->GetEntities().begin(); it != GetScene()->GetEntities().end(); ++it)
    {
        if (it->second->GetType() != pb::TypeHash("PlayerShip"))
            continue;
        
        pb::TransformComponent* transform = it->second->GetComponent<pb::TransformComponent>();
        
        return transform->GetPosition();
    }
    
    return glm::vec3(0,0,0);
}
