#pragma once

#include "pixelboost/logic/entity.h"

#include "gameplay/health.h"

namespace pb
{
    class Message;
    class Scene;
}

class Projectile : public pb::Entity
{
public:
	enum BehaviourType
	{
		kBehaviourTypeLaser,
        kBehaviourTypeBeam,
		kBehaviourTypeHoming,
	};

    Projectile(pb::Scene* scene, HealthType healthType, BehaviourType behaviour, glm::vec3 position, float rotation, float speed, float damage);
    ~Projectile();
    
public:
    virtual pb::Uid GetType() const;
    static pb::Uid GetStaticType();
    
private:
    void OnCollision(const pb::Message& message);
    void OnUpdate(const pb::Message& message);
    
    void UpdateBehaviour();
    void SetTarget();
    
private:
    BehaviourType _Behaviour;
    float _Life;
    float _Speed;
    pb::Uid _Target;
};
