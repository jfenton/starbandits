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
#include "gameplay/explosion.h"
#include "gameplay/health.h"
#include "player/player.h"
#include "screens/game.h"

PB_DEFINE_ENTITY(HomingMine)

HomingMine::HomingMine(pb::Scene* scene, pb::Entity* entity, pb::DbEntity* creationEntity)
	: pb::Entity(scene, entity, creationEntity)
    , _PlayerId(0)
{
    
}

HomingMine::~HomingMine()
{
    UnregisterMessageHandler<pb::UpdateMessage>(pb::MessageHandler(this, &HomingMine::OnUpdate));
    UnregisterMessageHandler<HealthDepletedMessage>(pb::MessageHandler(this, &HomingMine::OnHealthDepleted));
}

HomingMine* HomingMine::Initialise(glm::vec2 position)
{
    float size = 1.f;
    
    _DetectDistance = 15.f;
    _Rotation = 0.f;
    _Fuse = 2.f;
    
    auto transform = CreateComponent<pb::TransformComponent>();
    transform->SetPosition(glm::vec3(position, 0) + Game::Instance()->GetGameScreen()->GetLevelOffset());
    
    pb::ModelComponent* model = CreateComponent<pb::ModelComponent>();
    model->SetModel("/models/homingMine");
    model->SetMaterial("/materials/homingMine_armed");
    model->SetLocalTransform(glm::scale(glm::mat4x4(), glm::vec3(size, size, size)));
    model->SetLayer(kGraphicLayerEnemies);
    model->SetShader(Game::Instance()->GetLitShader());
    
    pb::PhysicsBody2DComponent* physics = CreateComponent<pb::PhysicsBody2DComponent>();
    physics->Initialise(pb::PhysicsBody2DComponent::kBodyTypeDynamic, pb::PhysicsBody2DComponent::kBodyShapeCircle, glm::vec2(size/2.f, size/2.f));
    
    physics->GetBody()->GetFixtureList()[0].SetDensity(20.f);
    physics->GetBody()->ResetMassData();
    
    CreateComponent<HealthComponent>()->Initialise(kHealthTypeEnemy, 5.f, 0.f);
    
    RegisterMessageHandler<pb::UpdateMessage>(pb::MessageHandler(this, &HomingMine::OnUpdate));
    RegisterMessageHandler<HealthDepletedMessage>(pb::MessageHandler(this, &HomingMine::OnHealthDepleted));
    
    return this;
}

void HomingMine::OnUpdate(const pb::Message& message)
{
    const pb::UpdateMessage& updateMessage = static_cast<const pb::UpdateMessage&>(message);
    _Fuse -= updateMessage.GetGameDelta();
    
    if (_Fuse > 0.f)
        return;
    
    glm::vec3 position = GetComponent<pb::TransformComponent>()->GetPosition();
    
    if (_PlayerId == 0)
    {
        pb::Scene::EntityMap entityMap = GetScene()->GetEntities();
        
        float maxDistance = 100.f;
        for (pb::Scene::EntityMap::iterator it = entityMap.begin(); it != entityMap.end(); ++it)
        {
            if (it->second->GetType() == GetStaticType() || it->second->GetType() == pb::TypeHash("Projectile")  || it->second->GetType() == pb::TypeHash("Cog"))
                continue;
            
            pb::TransformComponent* transform = it->second->GetComponent<pb::TransformComponent>();
            
            if (!transform)
                continue;
            
            glm::vec3 entityPosition = transform->GetPosition();
            
            if (glm::distance(position, entityPosition) < 3.f)
            {
                GetComponent<HealthComponent>()->Damage(10.f);
            }
            
            if (it->second->GetType() != PlayerShip::GetStaticType())
                continue;
            
            float distance = glm::distance(position, entityPosition);
            if (distance < _DetectDistance && distance < maxDistance)
            {
                _DetectDistance = 45.f;
                
                maxDistance = distance;
                _PlayerId = it->second->GetUid();
            }
        }
    }
    
    if (_PlayerId)
    {
        _Rotation += 10.f;
        
        pb::Entity* entity = GetScene()->GetEntityById(_PlayerId);
        
        if (!entity)
        {
            _PlayerId = 0;
            return;
        }
        
        pb::TransformComponent* transform = entity->GetComponent<pb::TransformComponent>();
        
        if (!transform)
        {
            _PlayerId = 0;
            return;
        }
        
        glm::vec3 entityPosition = transform->GetPosition();
        
        b2Body* body = GetComponent<pb::PhysicsBody2DComponent>()->GetBody();
        
        float maxSpeed = 8.f;
        float force = 150.f;
        glm::vec3 direction = glm::normalize(entityPosition-position)*force;
        body->ApplyForceToCenter(b2Vec2(direction.x, direction.y));
        
        glm::vec2 velocity(body->GetLinearVelocity().x, body->GetLinearVelocity().y);
        if (glm::length(velocity) > maxSpeed)
            velocity = glm::normalize(velocity) * maxSpeed;
        
        body->SetLinearVelocity(b2Vec2(velocity.x, velocity.y));
        
        if (glm::distance(position, entityPosition) < _DetectDistance)
        {
            _DetectDistance = 45.f;
            
            _PlayerId = 0;
        }
        
        GetComponent<pb::ModelComponent>()->SetMaterial("/materials/homingMine_active");
    } else {
        GetComponent<pb::ModelComponent>()->SetMaterial("/materials/homingMine_armed");
    }
    
    glm::mat4x4 transform;
    transform = glm::rotate(transform, _Rotation, glm::normalize(glm::vec3(0.4, 0.1, 0.7)));
    
    GetComponent<pb::ModelComponent>()->SetLocalTransform(transform);
}

void HomingMine::OnHealthDepleted(const pb::Message& message)
{
    glm::vec3 position = GetComponent<pb::TransformComponent>()->GetPosition();
    
    Destroy();
    GetScene()->CreateEntity<Explosion>(0,0)->Initialise(glm::vec2(position.x, position.y), 5.f);
}
