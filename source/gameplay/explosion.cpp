#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/random.hpp"

#include "pixelboost/audio/audioManagerSimple.h"
#include "pixelboost/graphics/particle/particleSystem.h"
#include "pixelboost/logic/component/graphics/ellipse.h"
#include "pixelboost/logic/component/physics/2d/physicsBody.h"
#include "pixelboost/logic/component/transform.h"
#include "pixelboost/logic/message/physics/collision.h"
#include "pixelboost/logic/message/update.h"
#include "pixelboost/logic/scene.h"

#include "common/layers.h"
#include "gameplay/cog.h"
#include "gameplay/damage.h"
#include "gameplay/explosion.h"

PB_DEFINE_ENTITY(Explosion)

Explosion::Explosion(pb::Scene* scene, pb::Entity* parent, pb::DbEntity* creationEntity)
    : pb::Entity(scene, parent, creationEntity)
    , _Power(0)
    , _Size(0)
    , _Time(0)
{
    
}

void Explosion::Initialise(glm::vec2 position, float power)
{
    _Power = power;
    
    auto transform = CreateComponent<pb::TransformComponent>();
    transform->SetPosition(glm::vec3(position, 0));
    
    if (power > 5.f) {
        pb::AudioManagerSimple::Instance()->PlaySfx("boom.wav", 0.6f);
    } else {
        pb::AudioManagerSimple::Instance()->PlaySfx("boom_low.wav", 0.5f);
    }

    CreateComponent<DamageComponent>()->Initialise(kHealthTypeNone, 5.f);
    
    auto physics = CreateComponent<pb::PhysicsBody2DComponent>();
    physics->Initialise(pb::PhysicsBody2DComponent::kBodyTypeDynamic, pb::PhysicsBody2DComponent::kBodyShapeCircle, glm::vec2(_Power/2.f, _Power/2.f));
    physics->SetSensor(true);
    
    auto particleComponent = CreateComponent<pb::ParticleComponent>();
    particleComponent->SetLayer(kGraphicLayerExplosions);
    
    pb::ParticleSystemDefinition* systemDefinition = particleComponent->GetSystem()->Definition;
    pb::ParticleDefinitionEmitterCone* emitter = new pb::ParticleDefinitionEmitterCone();
    emitter->EmitCount = 5.f;
    emitter->EmitSpeed = 30.f;
    emitter->Range = 180.f;
    systemDefinition->RenderSprite = new pb::ParticleSpriteDefinition("engine");
    systemDefinition->StartLife.Set(0.25f, 0.5f);
    systemDefinition->StartSpeed.Set(0.5f, 1.f);
    pb::ParticleValueCurve1D* scaleValue = new pb::ParticleValueCurve1D();
    scaleValue->Curve.Points.push_back(pb::HermiteCurve1D::Point(glm::vec2(-0.1,0), glm::vec2(0.f,7.5f), glm::vec2(0.5,0)));
    scaleValue->Curve.Points.push_back(pb::HermiteCurve1D::Point(glm::vec2(-0.2,0), glm::vec2(1.f,0.3f), glm::vec2(0.1,0)));
    systemDefinition->ModifierScale = scaleValue;
    systemDefinition->Emitter = emitter;
    
    particleComponent->SetSpriteSheet("/spritesheets/game");
    
    for (int i=0; i<=power; i+= 8.f)
    {
        GetScene()->CreateEntity<Cog>(0,0)->Initialise(position, glm::vec2(glm::linearRand(-1.f, 1.f), glm::linearRand(-1.f, 1.f)));
    }
    
    RegisterMessageHandler<pb::UpdateMessage>(pb::MessageHandler(this, &Explosion::OnUpdate));
}

Explosion::~Explosion()
{
    UnregisterMessageHandler<pb::UpdateMessage>(pb::MessageHandler(this, &Explosion::OnUpdate));
}

void Explosion::OnUpdate(const pb::Message& message)
{
    const pb::UpdateMessage& updateMessage = static_cast<const pb::UpdateMessage&>(message);
    
    _Size += (updateMessage.GetGameDelta()*15.f);
    _Time += updateMessage.GetGameDelta();
    
    if (_Size > _Power)
    {
        _Size = _Power;
        DestroyComponent(GetComponent<pb::PhysicsBody2DComponent>());
    }
    
    if (_Time > 3.f)
    {
        Destroy();
    }
}
