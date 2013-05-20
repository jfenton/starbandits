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
#include "enemies/asteroid.h"
#include "gameplay/health.h"
#include "screens/game.h"

PB_DEFINE_ENTITY(Asteroid)

Asteroid::Asteroid(pb::Scene* scene, pb::Entity* parent, pb::DbEntity* creationEntity)
    : pb::Entity(scene, parent, creationEntity)
{
    
}

void Asteroid::Initialise(glm::vec2 position, float scale)
{
    _Rotation = ((float)rand()/(float)RAND_MAX)*360.f;
    _Speed = ((float)rand()/(float)RAND_MAX)*1.5f;
    _Scale = scale;
    _Roll = 0.f;
    
    auto transform = CreateComponent<pb::TransformComponent>();
    transform->SetPosition(glm::vec3(position, 0) + Game::Instance()->GetGameScreen()->GetLevelOffset());
    
    char modelName[64];
    sprintf(modelName, "/models/asteroid%02d", (rand()%4)+1);
    
    pb::ModelComponent* model = CreateComponent<pb::ModelComponent>();
    model->SetModel(modelName);
    model->SetMaterial("/materials/asteroid");
    model->SetLocalTransform(glm::scale(glm::mat4x4(), glm::vec3(_Scale, _Scale, _Scale)));
    model->SetLayer(kGraphicLayerEnemies);
    model->SetShader(Game::Instance()->GetLitShader());
    
    auto physics = CreateComponent<pb::PhysicsBody2DComponent>();
    physics->Initialise(pb::PhysicsBody2DComponent::kBodyTypeDynamic, pb::PhysicsBody2DComponent::kBodyShapeCircle, glm::vec2(_Scale, _Scale));
    physics->GetBody()->SetLinearVelocity(b2Vec2(cos(_Rotation+glm::radians(90.f))*_Speed, sin(_Rotation+glm::radians(90.f))*_Speed));
    
    physics->GetBody()->GetFixtureList()[0].SetDensity(10.f);
    physics->GetBody()->ResetMassData();
    
    CreateComponent<HealthComponent>()->Initialise(kHealthTypeNone, _Scale * 450.f, 0.f);
    
    RegisterMessageHandler<pb::UpdateMessage>(pb::MessageHandler(this, &Asteroid::OnUpdate));
}

Asteroid::~Asteroid()
{
    if (_Scale > 1.f)
    {
        glm::vec3 position = GetComponent<pb::TransformComponent>()->GetPosition();
        GetScene()->CreateEntity<Asteroid>(0,0)->Initialise(glm::vec2(position.x, position.y), _Scale*0.5);
        GetScene()->CreateEntity<Asteroid>(0,0)->Initialise(glm::vec2(position.x, position.y), _Scale*0.5);
    }
    
    UnregisterMessageHandler<pb::UpdateMessage>(pb::MessageHandler(this, &Asteroid::OnUpdate));
}

void Asteroid::OnUpdate(const pb::Message& message)
{
    const pb::UpdateMessage& updateMessage = static_cast<const pb::UpdateMessage&>(message);
    
    _Roll += _Speed * 10.f * updateMessage.GetGameDelta();
    
    glm::mat4x4 transform;
    transform = glm::rotate(transform, _Roll, glm::normalize(glm::vec3(0.7,0.1,0.3)));
    transform = glm::scale(transform, glm::vec3(_Scale, _Scale, _Scale));
    GetComponent<pb::ModelComponent>()->SetLocalTransform(transform);
}
