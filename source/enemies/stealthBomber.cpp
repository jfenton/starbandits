#include "Box2D/Box2D.h"
#include "glm/gtc/matrix_transform.hpp"

#include "pixelboost/framework/engine.h"
#include "pixelboost/graphics/renderer/model/modelRenderer.h"
#include "pixelboost/logic/component/graphics/model.h"
#include "pixelboost/logic/component/physics/2d/physicsBody.h"
#include "pixelboost/logic/component/transform.h"
#include "pixelboost/logic/message/update.h"
#include "pixelboost/logic/scene.h"
#include "pixelboost/maths/matrixHelpers.h"

#include "common/layers.h"
#include "core/game.h"
#include "enemies/homingMine.h"
#include "enemies/stealthBomber.h"
#include "gameplay/explosion.h"
#include "gameplay/health.h"
#include "player/player.h"
#include "screens/game.h"

PB_DEFINE_ENTITY(StealthBomber)

StealthBomber::StealthBomber(pb::Scene* scene, pb::Entity* parent, pb::DbEntity* creationEntity)
	: pb::Entity(scene, parent, creationEntity)
    , _MineTimer(0)
{
    
}

StealthBomber::~StealthBomber()
{
    UnregisterMessageHandler<pb::UpdateMessage>(pb::MessageHandler(this, &StealthBomber::OnUpdate));
    UnregisterMessageHandler<HealthDepletedMessage>(pb::MessageHandler(this, &StealthBomber::OnHealthDepleted));
}

StealthBomber* StealthBomber::Initialise(glm::vec2 position)
{
    auto transform = CreateComponent<pb::TransformComponent>();
    transform->SetPosition(glm::vec3(position.x, position.y, 0));
    transform->SetRotation(glm::vec3(0,0,GetCreationEntity()->GetRotation().z+90.f));
    
    pb::ModelComponent* model = CreateComponent<pb::ModelComponent>();
    model->SetModel("/models/stealthBomber");
    model->SetMaterial("/materials/stealthBomber");
    model->SetLocalTransform(pb::CreateRotateMatrix(pb::kRotationOrder_XYZ, glm::vec3(90.f,90.f,0.f)));
    model->SetLayer(kGraphicLayerEnemies);
    
    pb::PhysicsBody2DComponent* physics = CreateComponent<pb::PhysicsBody2DComponent>();
    physics->Initialise(pb::PhysicsBody2DComponent::kBodyTypeDynamic, pb::PhysicsBody2DComponent::kBodyShapeCircle, glm::vec2(1.f, 1.f));
    
    physics->GetBody()->GetFixtureList()[0].SetDensity(20.f);
    physics->GetBody()->ResetMassData();
    
    auto health = CreateComponent<HealthComponent>();
    health->Initialise(kHealthTypeEnemy, 20.f, 0.f);
    
    RegisterMessageHandler<pb::UpdateMessage>(pb::MessageHandler(this, &StealthBomber::OnUpdate));
    RegisterMessageHandler<HealthDepletedMessage>(pb::MessageHandler(this, &StealthBomber::OnHealthDepleted));
    
    return this;
}

void StealthBomber::OnUpdate(const pb::Message& message)
{
    const pb::UpdateMessage& updateMessage = static_cast<const pb::UpdateMessage&>(message);
    
    pb::TransformComponent* transform = GetComponent<pb::TransformComponent>();
    
    glm::vec3 position = transform->GetPosition();
    
    b2Body* body = GetComponent<pb::PhysicsBody2DComponent>()->GetBody();
    
    float rotation = body->GetAngle() + updateMessage.GetGameDelta()*0.001f;
    
    body->SetTransform(body->GetPosition(), rotation);
    
    float maxSpeed = 2.f;
    float force = 200.f;
    glm::vec3 direction = glm::vec3(glm::cos(rotation), glm::sin(rotation), 0)*force;
    body->ApplyForceToCenter(b2Vec2(direction.x, direction.y));
    
    glm::vec2 velocity(body->GetLinearVelocity().x, body->GetLinearVelocity().y);
    if (glm::length(velocity) > maxSpeed)
        velocity = glm::normalize(velocity) * maxSpeed;

    body->SetLinearVelocity(b2Vec2(velocity.x, velocity.y));
    
    _MineTimer -= updateMessage.GetGameDelta();
    
    if (_MineTimer <= 0.f)
    {
        _MineTimer += 3.f;
        
        GetScene()->CreateEntity<HomingMine>(0,0)->Initialise(glm::vec2(position.x, position.y));
    }
}

void StealthBomber::OnHealthDepleted(const pb::Message& message)
{
    glm::vec3 position = GetComponent<pb::TransformComponent>()->GetPosition();
    
    Destroy();
    
    auto explosion = GetScene()->CreateEntity<Explosion>(0, 0);
    explosion->Initialise(glm::vec2(position.x, position.y), 2.f);
}
