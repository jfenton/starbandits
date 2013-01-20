#include "glm/gtc/matrix_transform.hpp"

#include "pixelboost/graphics/renderer/model/modelRenderer.h"
#include "pixelboost/logic/component/transform.h"
#include "pixelboost/logic/message/transform.h"
#include "pixelboost/logic/message/update.h"
#include "pixelboost/logic/system/graphics/render/render.h"
#include "pixelboost/logic/entity.h"
#include "pixelboost/logic/scene.h"
#include "pixelboost/maths/matrixHelpers.h"

#include "common/layers.h"
#include "core/game.h"
#include "player/weapon/missile.h"
#include "player/projectile.h"
#include "player/player.h"

MissileComponent::MissileComponent(pb::Entity* entity, PlayerInput* input, const MountInfo& mountInfo)
    : pb::Component(entity)
    , _Input(input)
    , _MountInfo(mountInfo)
    , _Fired(false)
{
    _Renderable = new pb::ModelRenderable(0);
    _Renderable->SetModel(Game::Instance()->GetModelRenderer()->GetModel("missile_01"));
    _Renderable->SetTexture(Game::Instance()->GetModelRenderer()->GetTexture("grey"));
    _Renderable->SetLayer(kGraphicLayerPlayer);
    _Renderable->SetShader(Game::Instance()->GetLitShader());
    
    GetParent()->RegisterMessageHandler<pb::TransformChangedMessage>(pb::Entity::MessageHandler(this, &MissileComponent::OnTransformChanged));
    GetParent()->RegisterMessageHandler<pb::UpdateMessage>(pb::Entity::MessageHandler(this, &MissileComponent::OnUpdate));
    GetScene()->GetSystemByType<pb::RenderSystem>()->AddItem(_Renderable);
    
    UpdateTransform();
}

MissileComponent::~MissileComponent()
{
    GetParent()->UnregisterMessageHandler<pb::TransformChangedMessage>(pb::Entity::MessageHandler(this, &MissileComponent::OnTransformChanged));
    GetParent()->UnregisterMessageHandler<pb::UpdateMessage>(pb::Entity::MessageHandler(this, &MissileComponent::OnUpdate));
    GetScene()->GetSystemByType<pb::RenderSystem>()->RemoveItem(_Renderable);
}

pb::Uid MissileComponent::GetType()
{
    return GetStaticType();
}

pb::Uid MissileComponent::GetStaticType()
{
    return pb::TypeHash("MissileComponent");
}

void MissileComponent::OnTransformChanged(const pb::Message& message)
{
    UpdateTransform();
}

void MissileComponent::OnUpdate(const pb::Message& message)
{
    PlayerShip* ship = static_cast<PlayerShip*>(GetParent());
    
    if (_MountInfo.IsLeft ? _Input->_FiringLeft : _Input->_FiringRight)
    {
        if (!_Fired)
        {
            _Fired = true;
            
            const float energyCost = 5.f;
            if (ship->GetEnergy() > energyCost)
            {
                pb::TransformComponent* transform = GetParent()->GetComponentByType<pb::TransformComponent>();
                
                glm::vec4 position = _Renderable->GetTransform() * glm::vec4(0,0,0.5,1);
                
                float randOffset = (((float)rand()/(float)RAND_MAX)-0.5)/6.f;
                new Projectile(GetScene(), kHealthTypePlayer, Projectile::kBehaviourTypeHoming, glm::vec3(position.x, position.y, position.z), glm::radians(transform->GetRotation().z) + randOffset, 5.f, 75.f);
                ship->RemoveEnergy(energyCost);
            }
        }
    } else {
        _Fired = false;
    }
}

void MissileComponent::UpdateTransform()
{
    PlayerShip* ship = static_cast<PlayerShip*>(GetParent());
    
    pb::TransformComponent* transform = GetParent()->GetComponentByType<pb::TransformComponent>();
    
    glm::mat4x4 localTransform;
    localTransform = glm::rotate(localTransform, ship->GetTilt(), glm::vec3(0,1,0));
    localTransform = localTransform * pb::CreateTransformMatrix(pb::kRotationOrder_XYZ, glm::vec3(_MountInfo.Offset), _MountInfo.Rotation + glm::vec3(90.f, 180.f, 0.f), glm::vec3(1.5, 1.5, 1.5));
    
    _Renderable->SetTransform(transform->GetMatrix() * localTransform);
}
