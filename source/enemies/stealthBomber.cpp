#include "Box2D/Box2D.h"
#include "glm/gtc/matrix_transform.hpp"

#include "pixelboost/framework/engine.h"
#include "pixelboost/graphics/renderer/model/modelRenderer.h"
#include "pixelboost/logic/component/graphics/model.h"
#include "pixelboost/logic/component/physics/2d/physicsBody.h"
#include "pixelboost/logic/component/transform/basic.h"
#include "pixelboost/logic/message/update.h"
#include "pixelboost/logic/scene.h"

#include "common/layers.h"
#include "core/game.h"
#include "enemies/homingMine.h"
#include "enemies/stealthBomber.h"
#include "gameplay/explosion.h"
#include "gameplay/health.h"
#include "player/player.h"

StealthBomber::StealthBomber(pb::Scene* scene, glm::vec2 position)
	: pb::Entity(scene, 0)
    , _MineTimer(0)
{
    pb::BasicTransformComponent* transform = new pb::BasicTransformComponent(this);
    transform->SetPosition(glm::vec3(position, 0));
    transform->SetRotation(glm::vec3(0,0,135));
    
    pb::ModelComponent* model = new pb::ModelComponent(this,
                                                       pb::Engine::Instance()->GetModelRenderer()->GetModel("stealthBomber"),
                                                       pb::Engine::Instance()->GetModelRenderer()->GetTexture("ship"));
    glm::mat4x4 localTransform;
    localTransform = glm::rotate(localTransform, 90.f, glm::vec3(1,0,0));
    localTransform = glm::rotate(localTransform, 90.f, glm::vec3(0,1,0));
    model->SetLocalTransform(localTransform);
    model->SetLayer(kGraphicLayerEnemies);
    model->SetShader(Game::Instance()->GetLitShader());
    
    pb::PhysicsBody2DComponent* physics = new pb::PhysicsBody2DComponent(this, pb::PhysicsBody2DComponent::kBodyTypeDynamic, pb::PhysicsBody2DComponent::kBodyShapeCircle, glm::vec2(1.f, 1.f));
    
    physics->GetBody()->GetFixtureList()[0].SetDensity(20.f);
    physics->GetBody()->ResetMassData();
    
    new HealthComponent(this, kHealthTypeEnemy, 20.f, 0.f);
    
    RegisterMessageHandler<pb::UpdateMessage>(MessageHandler(this, &StealthBomber::OnUpdate));
    RegisterMessageHandler<HealthDepletedMessage>(MessageHandler(this, &StealthBomber::OnHealthDepleted));
}

StealthBomber::~StealthBomber()
{
    UnregisterMessageHandler<pb::UpdateMessage>(MessageHandler(this, &StealthBomber::OnUpdate));
    UnregisterMessageHandler<HealthDepletedMessage>(MessageHandler(this, &StealthBomber::OnHealthDepleted));
}

pb::Uid StealthBomber::GetType() const
{
    return GetStaticType();
}

pb::Uid StealthBomber::GetStaticType()
{
    return pb::TypeHash("StealthBomber");
}

void StealthBomber::OnUpdate(const pb::Message& message)
{
    const pb::UpdateMessage& updateMessage = static_cast<const pb::UpdateMessage&>(message);
    
    pb::TransformComponent* transform = GetComponentByType<pb::TransformComponent>();
    
    glm::vec3 position = transform->GetPosition();
    
    b2Body* body = GetComponentByType<pb::PhysicsBody2DComponent>()->GetBody();
    
    float rotation = body->GetTransform().q.GetAngle() + updateMessage.GetDelta()*0.001f;
    
    body->SetTransform(body->GetPosition(), rotation);
    
    float maxSpeed = 2.f;
    float force = 200.f;
    glm::vec3 direction = glm::vec3(glm::cos(rotation), glm::sin(rotation), 0)*force;
    body->ApplyForceToCenter(b2Vec2(direction.x, direction.y));
    
    glm::vec2 velocity(body->GetLinearVelocity().x, body->GetLinearVelocity().y);
    if (glm::length(velocity) > maxSpeed)
        velocity = glm::normalize(velocity) * maxSpeed;

    body->SetLinearVelocity(b2Vec2(velocity.x, velocity.y));
    
    _MineTimer -= updateMessage.GetDelta();
    
    if (_MineTimer <= 0.f)
    {
        _MineTimer += 1.5f;
        
        new HomingMine(GetScene(), glm::vec2(position.x, position.y));
    }
}

void StealthBomber::OnHealthDepleted(const pb::Message& message)
{
    glm::vec3 position = GetComponentByType<pb::TransformComponent>()->GetPosition();
    
    Destroy();
    new Explosion(GetScene(), glm::vec2(position.x, position.y), 2.f);
}
