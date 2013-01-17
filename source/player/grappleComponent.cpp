#include "pixelboost/graphics/renderer/primitive/primitiveRenderer.h"
#include "pixelboost/logic/component/transform.h"
#include "pixelboost/logic/message/update.h"
#include "pixelboost/logic/system/graphics/render/render.h"
#include "pixelboost/logic/entity.h"
#include "pixelboost/logic/scene.h"

#include "common/layers.h"
#include "player/grappleComponent.h"

GrappleComponent::GrappleComponent(pb::Entity* entity, pb::Uid shipEntity, pb::Uid connectedEntity)
    : pb::Component(entity)
    , _ConnectedEntity(connectedEntity)
    , _ShipEntity(shipEntity)
{
    _Renderable = new pb::PrimitiveRenderableLine(GetUid());
    _Renderable->SetLayer(kGraphicLayerPlayer);
    UpdatePosition();
    
    GetParent()->RegisterMessageHandler<pb::UpdateMessage>(pb::Entity::MessageHandler(this, &GrappleComponent::OnUpdate));
    GetScene()->GetSystemByType<pb::RenderSystem>()->AddItem(_Renderable);
}

GrappleComponent::~GrappleComponent()
{
    GetParent()->UnregisterMessageHandler<pb::UpdateMessage>(pb::Entity::MessageHandler(this, &GrappleComponent::OnUpdate));
    GetScene()->GetSystemByType<pb::RenderSystem>()->RemoveItem(_Renderable);
    
    delete _Renderable;
}
    
pb::Uid GrappleComponent::GetType()
{
    return GetStaticType();
}

pb::Uid GrappleComponent::GetStaticType()
{
    return pb::TypeHash("GrappleComponent");
}
    
void GrappleComponent::OnUpdate(const pb::Message& message)
{
    UpdatePosition();
}

void GrappleComponent::UpdatePosition()
{
    pb::Entity* shipEntity = GetScene()->GetEntityById(_ShipEntity);
    pb::Entity* otherEntity = GetScene()->GetEntityById(_ConnectedEntity);
    
    if (!shipEntity || !otherEntity)
    {
        GetParent()->DestroyComponent(this);
        return;
    }
    
    pb::TransformComponent* shipTransform = shipEntity->GetComponentByType<pb::TransformComponent>();
    pb::TransformComponent* connectedTransform = otherEntity->GetComponentByType<pb::TransformComponent>();

    _Renderable->SetStart(shipTransform->GetPosition());
    _Renderable->SetEnd(connectedTransform->GetPosition());
}
