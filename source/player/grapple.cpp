#include "Box2D/Box2D.h"
#include "glm/gtc/matrix_transform.hpp"

#include "pixelboost/framework/engine.h"
#include "pixelboost/logic/component/graphics/rectangle.h"
#include "pixelboost/logic/component/physics/2d/physicsBody.h"
#include "pixelboost/logic/component/transform/basic.h"
#include "pixelboost/logic/system/physics/2d/physics.h"
#include "pixelboost/logic/message/physics/collision.h"
#include "pixelboost/logic/message/update.h"
#include "pixelboost/logic/scene.h"

#include "player/grappleComponent.h"
#include "player/grapple.h"
#include "player/player.h"

Grapple::Grapple(pb::Scene* scene, pb::Uid playerId, glm::vec3 position, float rotation, float speed)
    : pb::Entity(scene, 0)
    , _CollisionBody(0)
    , _Life(1.f)
    , _PlayerId(playerId)
    , _Rotation(rotation)
{
    pb::BasicTransformComponent* transform = new pb::BasicTransformComponent(this);
    transform->SetRotation(glm::vec3(0,0,glm::degrees(rotation)));
    transform->SetPosition(position);
    
    pb::RectangleComponent* rectangle = new pb::RectangleComponent(this);
    rectangle->SetSize(glm::vec2(0.1f, 0.5f));
    rectangle->SetColor(glm::vec4(108.f/255.f, 183.f/255.f, 227.f/255.f,1));
    rectangle->SetSolid(true);
    rectangle->SetLayer(1);
    
    pb::PhysicsBody2DComponent* physics = new pb::PhysicsBody2DComponent(this, pb::PhysicsBody2DComponent::kBodyTypeDynamic, pb::PhysicsBody2DComponent::kBodyShapeRect, glm::vec2(0.3,0.3));
    physics->SetSensor(true);
    physics->GetBody()->SetBullet(true);
    physics->GetBody()->SetLinearVelocity(b2Vec2(cos(rotation+glm::radians(90.f))*speed, sin(rotation+glm::radians(90.f))*speed));
    
    RegisterMessageHandler<pb::PhysicsCollisionMessage>(MessageHandler(this, &Grapple::OnCollision));
    RegisterMessageHandler<pb::UpdateMessage>(MessageHandler(this, &Grapple::OnUpdate));
}

Grapple::~Grapple()
{
    UnregisterMessageHandler<pb::PhysicsCollisionMessage>(MessageHandler(this, &Grapple::OnCollision));
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

void Grapple::OnCollision(const pb::Message& message)
{
    if (GetState() != kEntityCreated)
        return;
    
    const pb::PhysicsCollisionMessage& collisionMessage = static_cast<const pb::PhysicsCollisionMessage&>(message);
    
    pb::PhysicsComponent2D* otherPhysics = static_cast<pb::PhysicsComponent2D*>(collisionMessage.GetOtherComponent());

    if (otherPhysics->GetParent()->GetType() == pb::TypeHash("Asteroid"))
    {
        _CollisionBody = otherPhysics->GetBody(); // TODO *** POTENTIAL CRASH *** : Work out a better way of doing this, this could crash
        _CollisionObject = otherPhysics->GetParent()->GetUid();
        
        PlayerShip* ship = static_cast<PlayerShip*>(GetScene()->GetEntityById(_PlayerId));
        new GrappleComponent(ship, _CollisionObject);
        
        ship->SetGrappleObject(_CollisionObject);
        
        Destroy();
    }
}

void Grapple::OnUpdate(const pb::Message& message)
{
    const pb::UpdateMessage& updateMessage = static_cast<const pb::UpdateMessage&>(message);
    
    _Life -= updateMessage.GetDelta();
    
    if (_Life <= 0.f)
        Destroy();
    
    if (false)//_CollisionBody)
    {
        PlayerShip* ship = static_cast<PlayerShip*>(GetScene()->GetEntityById(_PlayerId));
        
        pb::PhysicsComponent2D* playerPhysics = ship->GetComponentByType<pb::PhysicsBody2DComponent>();
        
        glm::vec2 positionA = glm::vec2(playerPhysics->GetBody()->GetPosition().x, playerPhysics->GetBody()->GetPosition().y);
        glm::vec2 positionB = glm::vec2(_CollisionBody->GetPosition().x, _CollisionBody->GetPosition().y);
        
        b2World* world = GetScene()->GetSystemByType<pb::PhysicsSystem2D>()->GetPhysicsWorld();
        
        b2RopeJointDef ropeDef;
        ropeDef.bodyA = playerPhysics->GetBody();
        ropeDef.bodyB = _CollisionBody;
        ropeDef.maxLength = (ropeDef.bodyA->GetPosition() - ropeDef.bodyB->GetPosition()).Length() * 0.99f;
        ropeDef.localAnchorA = ropeDef.bodyB->GetWorldCenter();
        ropeDef.localAnchorB = ropeDef.bodyA->GetWorldCenter();
        
        b2DistanceJointDef distanceDef;
        distanceDef.bodyA = playerPhysics->GetBody();
        distanceDef.bodyB = _CollisionBody;
        distanceDef.length = glm::distance(positionA, positionB);
        
        b2Joint* rotationJoint = 0;//world->CreateJoint(&ropeDef);
        b2Joint* distanceJoint = 0;//world->CreateJoint(&distanceDef);
        
        ship->SetGrappleJoints(distanceJoint, rotationJoint);
        ship->SetGrappleObject(_CollisionObject);
        
        _CollisionBody = 0;
        
        Destroy();
    }
}
