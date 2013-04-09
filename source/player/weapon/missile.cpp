#include "glm/gtc/matrix_transform.hpp"

#include "pixelboost/audio/audioManagerSimple.h"
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

PB_DEFINE_COMPONENT(MissileComponent)

MissileComponent::MissileComponent(pb::Entity* entity)
    : pb::Component(entity)
    , _Fired(false)
    , _Input(0)
{
    
}

void MissileComponent::Initialise(PlayerInput* input, const MountInfo& mountInfo)
{
    _Input = input;
    _MountInfo = mountInfo;
    
    _Renderable = new pb::ModelRenderable();
    _Renderable->SetModel(pb::ModelRenderer::Instance()->GetModel("missile_01"));
    _Renderable->SetTexture(pb::ModelRenderer::Instance()->GetTexture("grey"));
    _Renderable->SetLayer(kGraphicLayerPlayer);
    _Renderable->SetShader(Game::Instance()->GetLitShader());
    
    GetEntity()->RegisterMessageHandler<pb::TransformChangedMessage>(pb::MessageHandler(this, &MissileComponent::OnTransformChanged));
    GetEntity()->RegisterMessageHandler<pb::UpdateMessage>(pb::MessageHandler(this, &MissileComponent::OnUpdate));
    GetScene()->GetSystemByType<pb::RenderSystem>()->AddItem(_Renderable);
    
    UpdateTransform();
}

MissileComponent::~MissileComponent()
{
    GetEntity()->UnregisterMessageHandler<pb::TransformChangedMessage>(pb::MessageHandler(this, &MissileComponent::OnTransformChanged));
    GetEntity()->UnregisterMessageHandler<pb::UpdateMessage>(pb::MessageHandler(this, &MissileComponent::OnUpdate));
    GetScene()->GetSystemByType<pb::RenderSystem>()->RemoveItem(_Renderable);
}

void MissileComponent::OnTransformChanged(const pb::Message& message)
{
    UpdateTransform();
}

void MissileComponent::OnUpdate(const pb::Message& message)
{
    PlayerShip* ship = static_cast<PlayerShip*>(GetEntity());
    
    if (_MountInfo.IsLeft ? _Input->_FiringLeft : _Input->_FiringRight)
    {
        if (!_Fired)
        {
            _Fired = true;
            
            const float energyCost = 5.f;
            if (ship->GetEnergy() > energyCost)
            {
                pb::TransformComponent* transform = GetEntity()->GetComponent<pb::TransformComponent>();
                
                glm::vec4 position = _Renderable->GetTransform() * glm::vec4(0,0,0.5,1);
                
                pb::AudioManagerSimple::Instance()->PlaySfx("missile.wav", 0.5f);
                
                float randOffset = (((float)rand()/(float)RAND_MAX)-0.5)/6.f;
                GetScene()->CreateEntity<Projectile>(0, 0)->Initialise(kHealthTypePlayer, kProjectileTypeHoming, glm::vec3(position.x, position.y, position.z), glm::radians(transform->GetRotation().z) + randOffset, 5.f, 75.f);
                ship->RemoveEnergy(energyCost);
            }
        }
    } else {
        _Fired = false;
    }
}

void MissileComponent::UpdateTransform()
{
    PlayerShip* ship = static_cast<PlayerShip*>(GetEntity());
    
    pb::TransformComponent* transform = GetEntity()->GetComponent<pb::TransformComponent>();
    
    glm::mat4x4 localTransform;
    localTransform = glm::rotate(localTransform, ship->GetTilt(), glm::vec3(0,1,0));
    localTransform = localTransform * pb::CreateTransformMatrix(pb::kRotationOrder_XYZ, glm::vec3(_MountInfo.Offset), _MountInfo.Rotation + glm::vec3(90.f, 180.f, 0.f), glm::vec3(1.5, 1.5, 1.5));
    
    _Renderable->SetTransform(transform->GetMatrix() * localTransform);
}
