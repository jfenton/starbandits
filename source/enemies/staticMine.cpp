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
#include "enemies/staticMine.h"
#include "gameplay/explosion.h"
#include "gameplay/health.h"
#include "player/player.h"

StaticMine::StaticMine(pb::Scene* scene, glm::vec2 position)
	: pb::Entity(scene, 0)
    , _Active(false)
    , _Countdown(1.f)
{
    float size = 1.f;
    
    _DetectDistance = 10.f;
    _Rotation = 0.f;
    
    pb::BasicTransformComponent* transform = new pb::BasicTransformComponent(this);
    transform->SetPosition(glm::vec3(position, 0));
    
    pb::ModelComponent* model = new pb::ModelComponent(this,
                                                       pb::Engine::Instance()->GetModelRenderer()->GetModel("staticMine"),
                                                       pb::Engine::Instance()->GetModelRenderer()->GetTexture("ship"));
    model->SetLocalTransform(glm::scale(glm::mat4x4(), glm::vec3(size, size, size)));
    model->SetLayer(kGraphicLayerEnemies);
    
    pb::PhysicsBody2DComponent* physics = new pb::PhysicsBody2DComponent(this, pb::PhysicsBody2DComponent::kBodyTypeDynamic, pb::PhysicsBody2DComponent::kBodyShapeCircle, glm::vec2(size/2.f, size/2.f));
    
    physics->GetBody()->GetFixtureList()[0].SetDensity(20.f);
    physics->GetBody()->ResetMassData();
    
    new HealthComponent(this, kHealthTypeEnemy, 15.f, 0.f);
    
    RegisterMessageHandler<pb::UpdateMessage>(MessageHandler(this, &StaticMine::OnUpdate));
    RegisterMessageHandler<HealthDepletedMessage>(MessageHandler(this, &StaticMine::OnHealthDepleted));
}

StaticMine::~StaticMine()
{
    UnregisterMessageHandler<pb::UpdateMessage>(MessageHandler(this, &StaticMine::OnUpdate));
    UnregisterMessageHandler<HealthDepletedMessage>(MessageHandler(this, &StaticMine::OnHealthDepleted));
}

pb::Uid StaticMine::GetType() const
{
    return GetStaticType();
}

pb::Uid StaticMine::GetStaticType()
{
    return pb::TypeHash("StaticMine");
}

void StaticMine::OnUpdate(const pb::Message& message)
{
    const pb::UpdateMessage& updateMessage = static_cast<const pb::UpdateMessage&>(message);

    if (_Active)
    {
        _Countdown -= updateMessage.GetDelta();
        _Rotation += updateMessage.GetDelta() * 180.f;

        if (_Countdown <= 0.f)
        {
            GetComponentByType<HealthComponent>()->ModifyHealth(-20.f);
        }
        
        GetComponentByType<pb::ModelComponent>()->SetLocalTransform(glm::rotate(glm::mat4x4(), _Rotation, glm::normalize(glm::vec3(0.2, 0.4, 0.1))));
    } else {
        glm::vec3 position = GetComponentByType<pb::TransformComponent>()->GetPosition();
        
        pb::Scene::EntityMap entityMap = GetScene()->GetEntitiesByType<PlayerShip>();
        
        for (pb::Scene::EntityMap::iterator it = entityMap.begin(); it != entityMap.end(); ++it)
        {
            glm::vec3 playerPosition = it->second->GetComponentByType<pb::TransformComponent>()->GetPosition();
            
            if (glm::distance(position, playerPosition) < _DetectDistance)
            {
                _Active = true;
            }
        }
    }
}

void StaticMine::OnHealthDepleted(const pb::Message& message)
{
    glm::vec3 position = GetComponentByType<pb::TransformComponent>()->GetPosition();
    
    Destroy();
    new Explosion(GetScene(), glm::vec2(position.x, position.y), 10.f);
}
