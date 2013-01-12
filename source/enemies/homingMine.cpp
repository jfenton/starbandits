#include "Box2D/Box2D.h"
#include "glm/gtc/matrix_transform.hpp"

#include "pixelboost/framework/engine.h"
#include "pixelboost/graphics/renderer/model/modelRenderer.h"
#include "pixelboost/logic/component/graphics/model.h"
#include "pixelboost/logic/component/physics/2d/physicsBody.h"
#include "pixelboost/logic/component/transform/basic.h"
#include "pixelboost/logic/message/update.h"
#include "pixelboost/logic/scene.h"

#include "enemies/homingMine.h"
#include "gameplay/health.h"
#include "player/player.h"

HomingMine::HomingMine(pb::Scene* scene, glm::vec2 position)
	: pb::Entity(scene, 0)
{
    float size = 1.f;
    
    _DetectDistance = 15.f;
    _Rotation = 0.f;
    
    pb::BasicTransformComponent* transform = new pb::BasicTransformComponent(this);
    transform->SetPosition(glm::vec3(position, 0));
    
    pb::ModelComponent* model = new pb::ModelComponent(this,
                                                       pb::Engine::Instance()->GetModelRenderer()->GetModel("homingMine"),
                                                       pb::Engine::Instance()->GetModelRenderer()->GetTexture("ship"));
    model->SetLocalTransform(glm::scale(glm::mat4x4(), glm::vec3(size, size, size)));
    model->SetLayer(1);
    
    pb::PhysicsBody2DComponent* physics = new pb::PhysicsBody2DComponent(this, pb::PhysicsBody2DComponent::kBodyTypeDynamic, pb::PhysicsBody2DComponent::kBodyShapeCircle, glm::vec2(size, size));
    
    physics->GetBody()->GetFixtureList()[0].SetDensity(10.f);
    physics->GetBody()->ResetMassData();
    
    new HealthComponent(this, -2, 20.f, 0.f);
    
    RegisterMessageHandler<pb::UpdateMessage>(MessageHandler(this, &HomingMine::OnUpdate));
}

HomingMine::~HomingMine()
{

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
    glm::vec3 position = GetComponentByType<pb::TransformComponent>()->GetPosition();
    
    pb::Scene::EntityMap entityMap = GetScene()->GetEntitiesByType<PlayerShip>();
    
    for (pb::Scene::EntityMap::iterator it = entityMap.begin(); it != entityMap.end(); ++it)
    {
        glm::vec3 playerPosition = it->second->GetComponentByType<pb::TransformComponent>()->GetPosition();
        
        if (glm::distance(position, playerPosition) < _DetectDistance)
        {
            _DetectDistance = 45.f;
            
            b2Body* body = GetComponentByType<pb::PhysicsBody2DComponent>()->GetBody();
            
            float maxSpeed = 3.5f;
            float force = 50.f;
            glm::vec3 direction = glm::normalize(playerPosition-position)*force;
            body->ApplyForceToCenter(b2Vec2(direction.x, direction.y));
            
            glm::vec2 velocity(body->GetLinearVelocity().x, body->GetLinearVelocity().y);
            if (glm::length(velocity) > maxSpeed)
                velocity = glm::normalize(velocity) * maxSpeed;

            body->SetLinearVelocity(b2Vec2(velocity.x, velocity.y));
        }
    }
}
