#include "Box2D/Box2D.h"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/random.hpp"

#include "pixelboost/framework/engine.h"
#include "pixelboost/graphics/renderer/model/modelRenderer.h"
#include "pixelboost/logic/component/graphics/model.h"
#include "pixelboost/logic/component/physics/2d/physicsBody.h"
#include "pixelboost/logic/component/transform.h"
#include "pixelboost/logic/message/update.h"
#include "pixelboost/logic/scene.h"

#include "common/layers.h"
#include "core/game.h"
#include "enemies/staticMine.h"
#include "gameplay/explosion.h"
#include "gameplay/health.h"
#include "player/player.h"
#include "screens/game.h"

PB_DEFINE_ENTITY(StaticMine)

StaticMine::StaticMine(pb::Scene* scene, pb::Entity* entity, pb::DbEntity* creationEntity)
	: pb::Entity(scene, entity, creationEntity)
    , _Active(false)
    , _Countdown(1.f)
{
    
}

StaticMine::~StaticMine()
{
    UnregisterMessageHandler<pb::UpdateMessage>(pb::MessageHandler(this, &StaticMine::OnUpdate));
    UnregisterMessageHandler<HealthDepletedMessage>(pb::MessageHandler(this, &StaticMine::OnHealthDepleted));
}

StaticMine* StaticMine::Initialise(glm::vec2 position)
{
    float size = 1.f;
    
    _DetectDistance = 10.f;
    _Rotation = glm::linearRand(0.f, 360.f);
    
    auto transform = CreateComponent<pb::TransformComponent>();
    transform->SetPosition(glm::vec3(position, 0) + Game::Instance()->GetGameScreen()->GetLevelOffset());
    
    pb::ModelComponent* model = CreateComponent<pb::ModelComponent>();
    model->SetModel("/models/staticMine");
    model->SetMaterial("/materials/staticMine");
    model->SetLocalTransform(glm::scale(glm::mat4x4(), glm::vec3(size, size, size)));
    model->SetLayer(kGraphicLayerEnemies);
    model->SetShader(Game::Instance()->GetLitShader());
    
    pb::PhysicsBody2DComponent* physics = CreateComponent<pb::PhysicsBody2DComponent>();
    physics->Initialise(pb::PhysicsBody2DComponent::kBodyTypeDynamic, pb::PhysicsBody2DComponent::kBodyShapeCircle, glm::vec2(size/2.f, size/2.f));
    physics->GetBody()->GetFixtureList()[0].SetDensity(20.f);
    physics->GetBody()->ResetMassData();
    
    CreateComponent<HealthComponent>()->Initialise(kHealthTypeEnemy, 15.f, 0.f);
    
    RegisterMessageHandler<pb::UpdateMessage>(pb::MessageHandler(this, &StaticMine::OnUpdate));
    RegisterMessageHandler<HealthDepletedMessage>(pb::MessageHandler(this, &StaticMine::OnHealthDepleted));
    
    return this;
}

void StaticMine::OnUpdate(const pb::Message& message)
{
    const pb::UpdateMessage& updateMessage = static_cast<const pb::UpdateMessage&>(message);

    if (_Active)
    {
        _Countdown -= updateMessage.GetGameDelta();
        _Rotation += updateMessage.GetGameDelta() * 180.f * (1.f-_Countdown)*7.f;

        if (_Countdown <= 0.f)
        {
            GetComponent<HealthComponent>()->Damage(20.f);
        }
        
        GetComponent<pb::ModelComponent>()->SetLocalTransform(glm::rotate(glm::mat4x4(), _Rotation, glm::normalize(glm::vec3(0.2, 0.4, 0.1))));
    } else {
        _Rotation += updateMessage.GetGameDelta() * 30.f;
        
        GetComponent<pb::ModelComponent>()->SetLocalTransform(glm::rotate(glm::mat4x4(), _Rotation, glm::normalize(glm::vec3(0.2, 0.4, 0.1))));
        
        glm::vec3 position = GetComponent<pb::TransformComponent>()->GetPosition();
        
        pb::Scene::EntityMap entityMap = GetScene()->GetEntitiesByType<PlayerShip>();
        
        for (pb::Scene::EntityMap::iterator it = entityMap.begin(); it != entityMap.end(); ++it)
        {
            glm::vec3 playerPosition = it->second->GetComponent<pb::TransformComponent>()->GetPosition();
            
            if (glm::distance(position, playerPosition) < _DetectDistance)
            {
                _Active = true;
                
                GetComponent<pb::ModelComponent>()->SetMaterial("/materials/staticMine_armed");
            }
        }
    }
}

void StaticMine::OnHealthDepleted(const pb::Message& message)
{
    glm::vec3 position = GetComponent<pb::TransformComponent>()->GetPosition();
    
    Destroy();
    GetScene()->CreateEntity<Explosion>(0,0)->Initialise(glm::vec2(position.x, position.y), 10.f);
}
