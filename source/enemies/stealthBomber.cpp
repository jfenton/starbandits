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
    auto transform = CreateComponent<pb::TransformComponent>();
    transform->SetPosition(glm::vec3(creationEntity->GetPosition().x, creationEntity->GetPosition().y, 0) + Game::Instance()->GetGameScreen()->GetLevelOffset());
    transform->SetRotation(glm::vec3(0,0,creationEntity->GetRotation().z));
    
    pb::ModelComponent* model = CreateComponent<pb::ModelComponent>();
    model->SetModel(pb::ModelRenderer::Instance()->GetModel("stealthBomber"));
    model->SetTexture(pb::ModelRenderer::Instance()->GetTexture("stealthBomber_DIFF"));
    
    glm::mat4x4 localTransform;
    localTransform = glm::rotate(localTransform, 90.f, glm::vec3(1,0,0));
    localTransform = glm::rotate(localTransform, 90.f, glm::vec3(0,1,0));
    model->SetLocalTransform(localTransform);
    model->SetLayer(kGraphicLayerEnemies);
    model->SetShader(Game::Instance()->GetLitShader());
    
    pb::PhysicsBody2DComponent* physics = CreateComponent<pb::PhysicsBody2DComponent>();
    physics->Initialise(pb::PhysicsBody2DComponent::kBodyTypeDynamic, pb::PhysicsBody2DComponent::kBodyShapeCircle, glm::vec2(1.f, 1.f));
    
    physics->GetBody()->GetFixtureList()[0].SetDensity(20.f);
    physics->GetBody()->ResetMassData();
    
    auto health = CreateComponent<HealthComponent>();
    health->Initialise(kHealthTypeEnemy, 20.f, 0.f);
    
    RegisterMessageHandler<pb::UpdateMessage>(pb::MessageHandler(this, &StealthBomber::OnUpdate));
    RegisterMessageHandler<HealthDepletedMessage>(pb::MessageHandler(this, &StealthBomber::OnHealthDepleted));
}

StealthBomber::~StealthBomber()
{
    UnregisterMessageHandler<pb::UpdateMessage>(pb::MessageHandler(this, &StealthBomber::OnUpdate));
    UnregisterMessageHandler<HealthDepletedMessage>(pb::MessageHandler(this, &StealthBomber::OnHealthDepleted));
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
