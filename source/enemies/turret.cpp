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
#include "enemies/turret.h"
#include "gameplay/explosion.h"
#include "gameplay/health.h"
#include "player/player.h"
#include "player/projectile.h"

Turret::Turret(pb::Scene* scene, glm::vec2 position)
    : pb::Entity(scene, 0)
    , _ShootTimer(0)
{
    pb::BasicTransformComponent* transform = new pb::BasicTransformComponent(this);
    transform->SetPosition(glm::vec3(position, 0));
    
    pb::ModelComponent* model = new pb::ModelComponent(this,
                                                       pb::Engine::Instance()->GetModelRenderer()->GetModel("turret"),
                                                       pb::Engine::Instance()->GetModelRenderer()->GetTexture("grey"));
    
    glm::mat4x4 localTransform;
    localTransform = glm::rotate(localTransform, 90.f, glm::vec3(1,0,0));
    localTransform = glm::rotate(localTransform, 180.f, glm::vec3(0,1,0));
    model->SetLocalTransform(localTransform);
    model->SetLayer(kGraphicLayerEnemies);
    model->SetShader(Game::Instance()->GetLitShader());
    
    pb::PhysicsBody2DComponent* physics = new pb::PhysicsBody2DComponent(this, pb::PhysicsBody2DComponent::kBodyTypeDynamic, pb::PhysicsBody2DComponent::kBodyShapeCircle, glm::vec2(1.f, 1.f));
    
    physics->GetBody()->GetFixtureList()[0].SetDensity(20.f);
    physics->GetBody()->ResetMassData();
    
    new HealthComponent(this, kHealthTypeEnemy, 100.f, 0.f);
    
    RegisterMessageHandler<pb::UpdateMessage>(MessageHandler(this, &Turret::OnUpdate));
    RegisterMessageHandler<HealthDepletedMessage>(MessageHandler(this, &Turret::OnHealthDepleted));
}

Turret::~Turret()
{
    UnregisterMessageHandler<pb::UpdateMessage>(MessageHandler(this, &Turret::OnUpdate));
    UnregisterMessageHandler<HealthDepletedMessage>(MessageHandler(this, &Turret::OnHealthDepleted));
}

pb::Uid Turret::GetType() const
{
    return GetStaticType();
}

pb::Uid Turret::GetStaticType()
{
    return pb::TypeHash("Turret");
}

void Turret::OnUpdate(const pb::Message& message)
{
    const pb::UpdateMessage& updateMessage = static_cast<const pb::UpdateMessage&>(message);
    
    pb::TransformComponent* transform = GetComponentByType<pb::TransformComponent>();
    glm::vec3 position = transform->GetPosition();
    
    b2Body* body = GetComponentByType<pb::PhysicsBody2DComponent>()->GetBody();
    body->SetTransform(body->GetPosition(), body->GetTransform().q.GetAngle() + updateMessage.GetDelta());
    
    _ShootTimer -= updateMessage.GetDelta();
    
    if (_ShootTimer <= 0.f)
    {
        new Projectile(GetScene(), kHealthTypeEnemy, Projectile::kBehaviourTypeLaser, position, body->GetTransform().q.GetAngle(), 10.f, 20.f);
        _ShootTimer = 0.5f;
    }
}

void Turret::OnHealthDepleted(const pb::Message& message)
{
    glm::vec3 position = GetComponentByType<pb::TransformComponent>()->GetPosition();
    
    Destroy();
    new Explosion(GetScene(), glm::vec2(position.x, position.y), 3.f);
}
