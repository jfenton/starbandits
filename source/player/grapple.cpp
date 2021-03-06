#include "Box2D/Box2D.h"
#include "glm/gtc/matrix_transform.hpp"

#include "pixelboost/framework/engine.h"
#include "pixelboost/graphics/renderer/primitive/primitiveRenderer.h"
#include "pixelboost/logic/component/graphics/rectangle.h"
#include "pixelboost/logic/component/physics/2d/physicsBody.h"
#include "pixelboost/logic/component/transform.h"
#include "pixelboost/logic/system/graphics/render/render.h"
#include "pixelboost/logic/system/physics/2d/physics.h"
#include "pixelboost/logic/message/physics/collision.h"
#include "pixelboost/logic/message/update.h"
#include "pixelboost/logic/scene.h"

#include "common/layers.h"
#include "player/grapple.h"
#include "player/player.h"

PB_DEFINE_ENTITY(Grapple)

Grapple::Grapple(pb::Scene* scene, pb::Entity* entity, pb::DbEntity* creationEntity)
    : pb::Entity(scene, entity, creationEntity)
    , _PlayerId(0)
    , _CollisionId(0)
    , _Input(0)
    , _Renderable(0)
{
    
}

void Grapple::Initialise(pb::Uid playerId, PlayerInput* input, pb::Uid collisionId)
{
    _CollisionId = collisionId;
    _Input = input;
    _PlayerId = playerId;

    _Renderable = new pb::PrimitiveRenderableLine();
    _Renderable->SetLayer(kGraphicLayerPlayer);
    _Renderable->SetColor(glm::vec4(116.f/255.f, 206.f/255.f, 236.f/255.f, 1.f));
    GetScene()->GetSystemByType<pb::RenderSystem>()->AddItem(_Renderable);
    
    _StartDistance = GetDistance();
    _StartDistance = glm::max(_StartDistance, 10.f);
    
    RegisterMessageHandler<pb::UpdateMessage>(pb::MessageHandler(this, &Grapple::OnUpdate));
}

Grapple::~Grapple()
{
    if (_Renderable)
        GetScene()->GetSystemByType<pb::RenderSystem>()->RemoveItem(_Renderable);
    
    delete _Renderable;

    UnregisterMessageHandler<pb::UpdateMessage>(pb::MessageHandler(this, &Grapple::OnUpdate));
}

void Grapple::OnUpdate(const pb::Message& message)
{
    UpdateData();
    
    PlayerShip* playerShip = static_cast<PlayerShip*>(GetScene()->GetEntityById(_PlayerId));
    
    if (!playerShip || playerShip->GetEnergy() < 0.1f)
    {
        Destroy();
        return;
    }
    
    playerShip->RemoveEnergy(0.1f);
    
    pb::Entity* grappleObject = GetScene()->GetEntityById(_CollisionId);
    
    b2Body* objectBody = grappleObject ? grappleObject->GetComponent<pb::PhysicsBody2DComponent>()->GetBody() : 0;
    
    if (!objectBody)
    {
        Destroy();
        return;
    }
   
    glm::vec2 direction = glm::vec2(_ObjectPosition.x-_PlayerPosition.x, _ObjectPosition.y-_PlayerPosition.y);
    
    _Renderable->SetStart(_ObjectPosition);
    _Renderable->SetEnd(_PlayerPosition);
    
    float objectForceScale = glm::min(glm::max(glm::distance(_ObjectPosition, _PlayerPosition)-_StartDistance, 0.f), 0.1f);
    
    b2Vec2 velocity = objectBody->GetLinearVelocity();
    velocity *= 0.9f;
    objectBody->SetLinearVelocity(velocity);
    
    objectBody->ApplyLinearImpulse(-b2Vec2(direction.x*objectBody->GetMass()*objectForceScale, direction.y*objectBody->GetMass()*objectForceScale), objectBody->GetPosition());
}

void Grapple::UpdateData()
{
    pb::Entity* grappleObject = GetScene()->GetEntityById(_CollisionId);
    PlayerShip* playerShip = static_cast<PlayerShip*>(GetScene()->GetEntityById(_PlayerId));
    
    if (!grappleObject || !playerShip || glm::length(_Input->_GrappleDirection) < 0.25f)
    {
        Destroy();
        return;
    }
    
    _ObjectPosition = grappleObject->GetComponent<pb::TransformComponent>()->GetPosition();
    _PlayerPosition = playerShip->GetComponent<pb::TransformComponent>()->GetPosition();
}

float Grapple::GetDistance()
{
    UpdateData();
    return glm::distance(_ObjectPosition, _PlayerPosition);
}
