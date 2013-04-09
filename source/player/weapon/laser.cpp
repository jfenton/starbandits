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
#include "player/weapon/laser.h"
#include "player/projectile.h"
#include "player/player.h"

PB_DEFINE_COMPONENT(LaserComponent)

LaserComponent::LaserComponent(pb::Entity* entity)
    : pb::Component(entity)
    , _FiringDelay(0)
    , _SoundDelay(0)
    , _Input(0)
{
    
}

void LaserComponent::Initialise(PlayerInput* input, const MountInfo& mountInfo)
{
    _Input = input;
    _MountInfo = mountInfo;
    
    _Renderable = new pb::ModelRenderable();
    _Renderable->SetModel(pb::ModelRenderer::Instance()->GetModel("weapon_laser"));
    _Renderable->SetTexture(pb::ModelRenderer::Instance()->GetTexture("weapon_laser"));
    _Renderable->SetLayer(kGraphicLayerPlayer);
    _Renderable->SetShader(Game::Instance()->GetLitShader());
    
    GetEntity()->RegisterMessageHandler<pb::TransformChangedMessage>(pb::MessageHandler(this, &LaserComponent::OnTransformChanged));
    GetEntity()->RegisterMessageHandler<pb::UpdateMessage>(pb::MessageHandler(this, &LaserComponent::OnUpdate));
    GetScene()->GetSystemByType<pb::RenderSystem>()->AddItem(_Renderable);
    
    UpdateTransform();
}

LaserComponent::~LaserComponent()
{
    GetEntity()->UnregisterMessageHandler<pb::TransformChangedMessage>(pb::MessageHandler(this, &LaserComponent::OnTransformChanged));
    GetEntity()->UnregisterMessageHandler<pb::UpdateMessage>(pb::MessageHandler(this, &LaserComponent::OnUpdate));
    GetScene()->GetSystemByType<pb::RenderSystem>()->RemoveItem(_Renderable);
}

void LaserComponent::OnTransformChanged(const pb::Message& message)
{
    UpdateTransform();
}

void LaserComponent::OnUpdate(const pb::Message& message)
{
    const pb::UpdateMessage& updateMessage = static_cast<const pb::UpdateMessage&>(message);
    
    PlayerShip* ship = static_cast<PlayerShip*>(GetEntity());
    
    _SoundDelay = glm::max(_SoundDelay-updateMessage.GetGameDelta(), 0.f);
    
    _FiringDelay = glm::max(0.f, _FiringDelay-updateMessage.GetGameDelta());
    if (_MountInfo.IsLeft ? _Input->_FiringLeft : _Input->_FiringRight)
    {
        if (_FiringDelay <= 0.f)
        {
            _FiringDelay += 0.08f;
            
            const float energyCost = 0.3f;
            if (ship->GetEnergy() > energyCost)
            {
                pb::TransformComponent* transform = GetEntity()->GetComponent<pb::TransformComponent>();
                
                glm::vec4 position = _Renderable->GetTransform() * glm::vec4(0,0,0.5,1);
                
                if (_SoundDelay <= 0.f)
                {
                    pb::AudioManagerSimple::Instance()->PlaySfx("laser.wav", 0.5f);
                    _SoundDelay = 0.25f;
                }
                
                float randOffset = (((float)rand()/(float)RAND_MAX)-0.5)/6.f;
                GetScene()->CreateEntity<Projectile>(0, 0)->Initialise(kHealthTypePlayer, kProjectileTypeLaser, glm::vec3(position.x, position.y, position.z), glm::radians(transform->GetRotation().z) + randOffset, 40.f, 5.f);
                ship->RemoveEnergy(energyCost);
            }
            
        }
    }
}

void LaserComponent::UpdateTransform()
{
    PlayerShip* ship = static_cast<PlayerShip*>(GetEntity());
    
    pb::TransformComponent* transform = GetEntity()->GetComponent<pb::TransformComponent>();
    
    glm::mat4x4 localTransform;
    localTransform = glm::rotate(localTransform, ship->GetTilt(), glm::vec3(0,1,0));
    localTransform = localTransform * pb::CreateTransformMatrix(pb::kRotationOrder_XYZ, glm::vec3(_MountInfo.Offset), _MountInfo.Rotation + glm::vec3(90.f, 180.f, 0.f), glm::vec3(1.5, 1.5, 1.5));
    
    _Renderable->SetTransform(transform->GetMatrix() * localTransform);
}
