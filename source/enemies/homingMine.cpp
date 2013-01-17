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
#include "gameplay/explosion.h"
#include "gameplay/health.h"
#include "player/player.h"

HomingMine::HomingMine(pb::Scene* scene, glm::vec2 position)
	: pb::Entity(scene, 0)
{
    float size = 1.f;
    
    _DetectDistance = 15.f;
    _Rotation = 0.f;
    _Fuse = 2.f;
    
    pb::BasicTransformComponent* transform = new pb::BasicTransformComponent(this);
    transform->SetPosition(glm::vec3(position, 0));
    
    pb::ModelComponent* model = new pb::ModelComponent(this,
                                                       pb::Engine::Instance()->GetModelRenderer()->GetModel("homingMine"),
                                                       pb::Engine::Instance()->GetModelRenderer()->GetTexture("grey"));
    model->SetLocalTransform(glm::scale(glm::mat4x4(), glm::vec3(size, size, size)));
    model->SetLayer(kGraphicLayerEnemies);
    model->SetShader(Game::Instance()->GetLitShader());
    
    pb::PhysicsBody2DComponent* physics = new pb::PhysicsBody2DComponent(this, pb::PhysicsBody2DComponent::kBodyTypeDynamic, pb::PhysicsBody2DComponent::kBodyShapeCircle, glm::vec2(size/2.f, size/2.f));
    
    physics->GetBody()->GetFixtureList()[0].SetDensity(20.f);
    physics->GetBody()->ResetMassData();
    
    new HealthComponent(this, kHealthTypeEnemy, 5.f, 0.f);
    
    RegisterMessageHandler<pb::UpdateMessage>(MessageHandler(this, &HomingMine::OnUpdate));
    RegisterMessageHandler<HealthDepletedMessage>(MessageHandler(this, &HomingMine::OnHealthDepleted));
}

HomingMine::~HomingMine()
{
    UnregisterMessageHandler<pb::UpdateMessage>(MessageHandler(this, &HomingMine::OnUpdate));
    UnregisterMessageHandler<HealthDepletedMessage>(MessageHandler(this, &HomingMine::OnHealthDepleted));
}

pb::Uid HomingMine::GetType() const
{
    return GetStaticType();
}

pb::Uid HomingMine::GetStaticType()
{
    return pb::TypeHash("HomingMine");
}

void HomingMine::OnUpdate(const pb::Message& message)
{
    const pb::UpdateMessage& updateMessage = static_cast<const pb::UpdateMessage&>(message);
    _Fuse -= updateMessage.GetDelta();
    
    if (_Fuse > 0.f)
        return;
    
    glm::vec3 position = GetComponentByType<pb::TransformComponent>()->GetPosition();
    
    pb::Scene::EntityMap entityMap = GetScene()->GetEntities();
    
    for (pb::Scene::EntityMap::iterator it = entityMap.begin(); it != entityMap.end(); ++it)
    {
        if (it->second->GetType() == GetStaticType())
            continue;
        
        pb::TransformComponent* transform = it->second->GetComponentByType<pb::TransformComponent>();
        
        if (!transform)
            continue;
        
        glm::vec3 entityPosition = transform->GetPosition();
        
        if (glm::distance(position, entityPosition) < 3.f)
        {
            GetComponentByType<HealthComponent>()->Damage(10.f);
        }
        
        if (it->second->GetType() != PlayerShip::GetStaticType())
            continue;
        
        if (glm::distance(position, entityPosition) < _DetectDistance)
        {
            _DetectDistance = 45.f;
            
            b2Body* body = GetComponentByType<pb::PhysicsBody2DComponent>()->GetBody();
            
            float maxSpeed = 8.f;
            float force = 150.f;
            glm::vec3 direction = glm::normalize(entityPosition-position)*force;
            body->ApplyForceToCenter(b2Vec2(direction.x, direction.y));
            
            glm::vec2 velocity(body->GetLinearVelocity().x, body->GetLinearVelocity().y);
            if (glm::length(velocity) > maxSpeed)
                velocity = glm::normalize(velocity) * maxSpeed;

            body->SetLinearVelocity(b2Vec2(velocity.x, velocity.y));
        }
    }
}

void HomingMine::OnHealthDepleted(const pb::Message& message)
{
    glm::vec3 position = GetComponentByType<pb::TransformComponent>()->GetPosition();
    
    Destroy();
    new Explosion(GetScene(), glm::vec2(position.x, position.y), 5.f);
}
