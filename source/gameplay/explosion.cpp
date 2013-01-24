#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/random.hpp"

#include "pixelboost/audio/soundManager.h"
#include "pixelboost/graphics/particle/particleSystem.h"
#include "pixelboost/logic/component/graphics/ellipse.h"
#include "pixelboost/logic/component/physics/2d/physicsBody.h"
#include "pixelboost/logic/component/transform/basic.h"
#include "pixelboost/logic/message/physics/collision.h"
#include "pixelboost/logic/message/update.h"

#include "common/layers.h"
#include "gameplay/cog.h"
#include "gameplay/damage.h"
#include "gameplay/explosion.h"

Explosion::Explosion(pb::Scene* scene, glm::vec2 position, float power)
    : pb::Entity(scene, 0)
    , _Power(power)
    , _Size(0)
    , _Time(0)
{
    pb::BasicTransformComponent* transform = new pb::BasicTransformComponent(this);
    transform->SetPosition(glm::vec3(position, 0));
    
    if (power > 5.f) {
        pb::SoundManager::Instance()->PlaySfx("boom.wav", 0.6f);
    } else {
        pb::SoundManager::Instance()->PlaySfx("boom_low.wav", 0.5f);
    }

    new DamageComponent(this, kHealthTypeNone, 5.f);
    
    pb::PhysicsBody2DComponent* physics = new pb::PhysicsBody2DComponent(this, pb::PhysicsBody2DComponent::kBodyTypeDynamic, pb::PhysicsBody2DComponent::kBodyShapeCircle, glm::vec2(_Power/2.f, _Power/2.f));

    physics->SetSensor(true);
    
    pb::ParticleComponent* particleComponent = new pb::ParticleComponent(this);
    particleComponent->SetLayer(kGraphicLayerExplosions);
    
    pb::ParticleSystemDefinition* engineDefinition = particleComponent->GetSystem()->Definition;
    pb::ParticleDefinitionEmitterCone* emitter = new pb::ParticleDefinitionEmitterCone();
    emitter->EmitCount = 5.f;
    emitter->EmitSpeed = 30.f;
    emitter->Range = 180.f;
    engineDefinition->RenderSprite = new pb::ParticleSpriteDefinition("engine");
    engineDefinition->StartLife.Set(0.25f, 0.5f);
    engineDefinition->StartSpeed.Set(0.5f, 1.f);
    pb::ParticleValueCurve1D* scaleValue = new pb::ParticleValueCurve1D();
    scaleValue->Curve.Points.push_back(pb::HermiteCurve2D::Point(glm::vec2(-0.1,0), glm::vec2(0.f,7.5f), glm::vec2(0.5,0)));
    scaleValue->Curve.Points.push_back(pb::HermiteCurve2D::Point(glm::vec2(-0.2,0), glm::vec2(1.f,0.3f), glm::vec2(0.1,0)));
    engineDefinition->ModifierScale = scaleValue;
    engineDefinition->Emitter = emitter;
    
    for (int i=0; i<=power; i+= 8.f)
    {
        new Cog(GetScene(), position, glm::vec2(glm::linearRand(-1.f, 1.f), glm::linearRand(-1.f, 1.f)));
    }
    
    RegisterMessageHandler<pb::UpdateMessage>(MessageHandler(this, &Explosion::OnUpdate));
}

Explosion::~Explosion()
{
    UnregisterMessageHandler<pb::UpdateMessage>(MessageHandler(this, &Explosion::OnUpdate));
}

pb::Uid Explosion::GetType() const
{
    return GetStaticType();
}

pb::Uid Explosion::GetStaticType()
{
    return pb::TypeHash("Explosion");
}

void Explosion::OnUpdate(const pb::Message& message)
{
    const pb::UpdateMessage& updateMessage = static_cast<const pb::UpdateMessage&>(message);
    
    _Size += (updateMessage.GetDelta()*15.f);
    _Time += updateMessage.GetDelta();
    
    if (_Size > _Power)
    {
        _Size = _Power;
        DestroyComponent(GetComponentByType<pb::PhysicsBody2DComponent>());
    }
    
    if (_Time > 3.f)
    {
        Destroy();
    }
}
