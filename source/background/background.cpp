#include "Box2D/Box2D.h"

#include "pixelboost/framework/engine.h"
#include "pixelboost/logic/component/graphics/sprite.h"
#include "pixelboost/logic/component/physics/2d/physicsBody.h"
#include "pixelboost/logic/component/transform/basic.h"
#include "pixelboost/logic/message/update.h"

#include "background/background.h"

BackgroundTile::BackgroundTile(pb::Scene* scene, glm::vec2 position)
    : pb::Entity(scene, 0)
{
    pb::BasicTransformComponent* transform = new pb::BasicTransformComponent(this);
    transform->SetPosition(glm::vec3(position, 0));
    
    new pb::SpriteComponent(this, "starfield");
}

BackgroundTile::~BackgroundTile()
{
    
}

pb::Uid BackgroundTile::GetType() const
{
    return GetStaticType();
}

pb::Uid BackgroundTile::GetStaticType()
{
    return pb::TypeHash("BackgroundTile");
}
