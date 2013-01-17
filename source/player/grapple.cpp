#include "Box2D/Box2D.h"
#include "glm/gtc/matrix_transform.hpp"

#include "pixelboost/framework/engine.h"
#include "pixelboost/graphics/renderer/primitive/primitiveRenderer.h"
#include "pixelboost/logic/component/graphics/rectangle.h"
#include "pixelboost/logic/component/physics/2d/physicsBody.h"
#include "pixelboost/logic/component/transform/basic.h"
#include "pixelboost/logic/system/graphics/render/render.h"
#include "pixelboost/logic/system/physics/2d/physics.h"
#include "pixelboost/logic/message/physics/collision.h"
#include "pixelboost/logic/message/update.h"
#include "pixelboost/logic/scene.h"

#include "common/layers.h"
#include "player/grappleComponent.h"
#include "player/grapple.h"
#include "player/player.h"

Grapple::Grapple(pb::Scene* scene, pb::Uid playerId, pb::Uid collisionId)
    : pb::Entity(scene, 0)
    , _CollisionId(collisionId)
    , _PlayerId(playerId)
    , _Renderable(0)
{
    _Renderable = new pb::PrimitiveRenderableLine(GetUid());
    _Renderable->SetLayer(kGraphicLayerPlayer);
    GetScene()->GetSystemByType<pb::RenderSystem>()->AddItem(_Renderable);
    
    RegisterMessageHandler<pb::UpdateMessage>(MessageHandler(this, &Grapple::OnUpdate));
}

Grapple::~Grapple()
{
    if (_Renderable)
        GetScene()->GetSystemByType<pb::RenderSystem>()->RemoveItem(_Renderable);
    
    delete _Renderable;

    UnregisterMessageHandler<pb::UpdateMessage>(MessageHandler(this, &Grapple::OnUpdate));
}

pb::Uid Grapple::GetType() const
{
    return GetStaticType();
}

pb::Uid Grapple::GetStaticType()
{
    return pb::TypeHash("Grapple");
}

void Grapple::OnUpdate(const pb::Message& message)
{
    const pb::UpdateMessage& updateMessage = static_cast<const pb::UpdateMessage&>(message);
    
    pb::Entity* grappleObject = GetScene()->GetEntityById(_CollisionId);
    PlayerShip* playerShip = static_cast<PlayerShip*>(GetScene()->GetEntityById(_PlayerId));
    
    if (!grappleObject || !playerShip)
    {
        Destroy();
        return;
    }
    
    b2Body* objectBody = grappleObject->GetComponentByType<pb::PhysicsBody2DComponent>()->GetBody();
        
    if (!objectBody)
    {
        Destroy();
        return;
    }
    glm::vec3 objectPosition = grappleObject->GetComponentByType<pb::TransformComponent>()->GetPosition();
    glm::vec3 playerPosition = playerShip->GetComponentByType<pb::TransformComponent>()->GetPosition();
    glm::vec2 direction = glm::vec2(objectPosition.x-playerPosition.x, objectPosition.y-playerPosition.y);
    
    _Renderable->SetStart(objectPosition);
    _Renderable->SetEnd(playerPosition);
    
    if (glm::distance(objectPosition, playerPosition) > 15.f)
        Destroy();
    
    float objectForceScale = -0.1f;
    
    b2Vec2 velocity = objectBody->GetLinearVelocity();
    velocity *= 0.9f;
    objectBody->SetLinearVelocity(velocity);
    objectBody->ApplyLinearImpulse(-b2Vec2(direction.x*objectBody->GetMass()*objectForceScale, direction.y*objectBody->GetMass()*objectForceScale), objectBody->GetPosition());
}
