#include "Box2D/Box2D.h"
#include "glm/gtc/matrix_transform.hpp"

#include "pixelboost/framework/engine.h"
#include "pixelboost/graphics/renderer/model/modelRenderer.h"
#include "pixelboost/logic/component/graphics/model.h"
#include "pixelboost/logic/component/physics/2d/physicsBody.h"
#include "pixelboost/logic/component/transform/basic.h"
#include "pixelboost/logic/message/update.h"

#include "common/layers.h"
#include "enemies/asteroid.h"
#include "gameplay/health.h"

Asteroid::Asteroid(pb::Scene* scene, glm::vec2 position, float scale)
    : pb::Entity(scene, 0)
{
    _Rotation = ((float)rand()/(float)RAND_MAX)*360.f;
    _Speed = ((float)rand()/(float)RAND_MAX)*3.f;
    _Scale = scale;
    _Size = (1.5f + ((float)rand()/(float)RAND_MAX)*2.f)*scale;
    
    pb::BasicTransformComponent* transform = new pb::BasicTransformComponent(this);
    transform->SetPosition(glm::vec3(position, 0));
    
    char modelName[64];
    sprintf(modelName, "asteroid_%02d", (rand()%4)+1);
    
    printf("Creating asteroid %s\n", modelName);
    
    pb::ModelComponent* model = new pb::ModelComponent(this,
                                                       pb::Engine::Instance()->GetModelRenderer()->GetModel(modelName),
                                                       pb::Engine::Instance()->GetModelRenderer()->GetTexture("asteroid"));
    model->SetLocalTransform(glm::scale(glm::mat4x4(), glm::vec3(_Size, _Size, _Size)));
    model->SetLayer(kGraphicLayerEnemies);
    
    pb::PhysicsBody2DComponent* physics = new pb::PhysicsBody2DComponent(this, pb::PhysicsBody2DComponent::kBodyTypeDynamic, pb::PhysicsBody2DComponent::kBodyShapeCircle, glm::vec2(_Size, _Size));
    physics->GetBody()->SetLinearVelocity(b2Vec2(cos(_Rotation+glm::radians(90.f))*_Speed, sin(_Rotation+glm::radians(90.f))*_Speed));
    
    physics->GetBody()->GetFixtureList()[0].SetDensity(10.f);
    physics->GetBody()->ResetMassData();
    
    new HealthComponent(this, kHealthTypeNone, _Scale * 50.f, 0.f);
    
    RegisterMessageHandler<pb::UpdateMessage>(MessageHandler(this, &Asteroid::OnUpdate));
}

Asteroid::~Asteroid()
{
    if (_Size > 1.f)
    {
        glm::vec3 position = GetComponentByType<pb::TransformComponent>()->GetPosition();
        new Asteroid(GetScene(), glm::vec2(position.x, position.y), _Scale*0.5);
        new Asteroid(GetScene(), glm::vec2(position.x, position.y), _Scale*0.5);
    }
    
    UnregisterMessageHandler<pb::UpdateMessage>(MessageHandler(this, &Asteroid::OnUpdate));
}

pb::Uid Asteroid::GetType() const
{
    return GetStaticType();
}

pb::Uid Asteroid::GetStaticType()
{
    return pb::TypeHash("Asteroid");
}

void Asteroid::OnUpdate(const pb::Message& message)
{
    const pb::UpdateMessage& updateMessage = static_cast<const pb::UpdateMessage&>(message);
    
    _Roll += _Speed * 10.f * updateMessage.GetDelta();
    
    glm::mat4x4 transform;
    transform = glm::rotate(transform, _Roll, glm::normalize(glm::vec3(0.7,0.1,0.3)));
    transform = glm::scale(transform, glm::vec3(_Size, _Size, _Size));
    GetComponentByType<pb::ModelComponent>()->SetLocalTransform(transform);
}
