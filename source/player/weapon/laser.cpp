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
#include "player/weapon/laser.h"
#include "player/projectile.h"
#include "player/player.h"

LaserComponent::LaserComponent(pb::Entity* entity, PlayerInput* input, const MountInfo& mountInfo)
    : pb::Component(entity)
    , _Input(input)
    , _MountInfo(mountInfo)
    , _FiringDelay(0)
{
    _Renderable = new pb::ModelRenderable(0);
    _Renderable->SetModel(Game::Instance()->GetModelRenderer()->GetModel("weapon_laser"));
    _Renderable->SetTexture(Game::Instance()->GetModelRenderer()->GetTexture("grey"));
    _Renderable->SetLayer(kGraphicLayerPlayer);
    _Renderable->SetShader(Game::Instance()->GetLitShader());
    
    GetParent()->RegisterMessageHandler<pb::TransformChangedMessage>(pb::Entity::MessageHandler(this, &LaserComponent::OnTransformChanged));
    GetParent()->RegisterMessageHandler<pb::UpdateMessage>(pb::Entity::MessageHandler(this, &LaserComponent::OnUpdate));
    GetScene()->GetSystemByType<pb::RenderSystem>()->AddItem(_Renderable);
    
    UpdateTransform();
}

LaserComponent::~LaserComponent()
{
    GetParent()->UnregisterMessageHandler<pb::UpdateMessage>(pb::Entity::MessageHandler(this, &LaserComponent::OnUpdate));
    GetScene()->GetSystemByType<pb::RenderSystem>()->RemoveItem(_Renderable);
}

pb::Uid LaserComponent::GetType()
{
    return GetStaticType();
}

pb::Uid LaserComponent::GetStaticType()
{
    return pb::TypeHash("LaserComponent");
}

void LaserComponent::OnTransformChanged(const pb::Message& message)
{
    UpdateTransform();
}

void LaserComponent::OnUpdate(const pb::Message& message)
{
    const pb::UpdateMessage& updateMessage = static_cast<const pb::UpdateMessage&>(message);
    
    PlayerShip* ship = static_cast<PlayerShip*>(GetParent());
    
    _FiringDelay = glm::max(0.f, _FiringDelay-updateMessage.GetDelta());
    if (_Input->_Firing)
    {
        if (_FiringDelay <= 0.f)
        {
            _FiringDelay += 0.08f;
            
            const float energyCost = 0.3f;
            if (ship->GetEnergy() > energyCost)
            {
                pb::TransformComponent* transform = GetParent()->GetComponentByType<pb::TransformComponent>();
                
                glm::vec4 position = _Renderable->GetTransform() * glm::vec4(0,0,0.5,1);
                
                float randOffset = (((float)rand()/(float)RAND_MAX)-0.5)/6.f;
                new Projectile(GetScene(), kHealthTypePlayer, glm::vec3(position.x, position.y, position.z), glm::radians(transform->GetRotation().z) + randOffset, 40.f, 5.f);
                ship->RemoveEnergy(energyCost);
            }
            
        }
    }
}

void LaserComponent::UpdateTransform()
{
    PlayerShip* ship = static_cast<PlayerShip*>(GetParent());
    
    pb::TransformComponent* transform = GetParent()->GetComponentByType<pb::TransformComponent>();
    
    glm::mat4x4 localTransform;
    localTransform = glm::rotate(localTransform, ship->GetTilt(), glm::vec3(0,1,0));
    localTransform = localTransform * pb::CreateTransformMatrix(pb::kRotationOrder_XYZ, glm::vec3(_MountInfo.Offset), _MountInfo.Rotation + glm::vec3(90.f, 180.f, 0.f), glm::vec3(1.5, 1.5, 1.5));
    
    _Renderable->SetTransform(transform->GetMatrix() * localTransform);
}
