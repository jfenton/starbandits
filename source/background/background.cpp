#include "Box2D/Box2D.h"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/random.hpp"

#include "pixelboost/framework/engine.h"
#include "pixelboost/graphics/device/device.h"
#include "pixelboost/graphics/device/indexBuffer.h"
#include "pixelboost/graphics/device/vertexBuffer.h"
#include "pixelboost/graphics/renderer/model/modelRenderer.h"
#include "pixelboost/graphics/renderer/sprite/spriteRenderer.h"
#include "pixelboost/logic/component/graphics/buffer.h"
#include "pixelboost/logic/component/graphics/model.h"
#include "pixelboost/logic/component/physics/2d/physicsBody.h"
#include "pixelboost/logic/component/transform/basic.h"
#include "pixelboost/logic/message/update.h"
#include "pixelboost/logic/system/graphics/render/render.h"
#include "pixelboost/logic/scene.h"

#include "background/background.h"
#include "common/layers.h"

BackgroundTile::BackgroundTile(pb::Scene* scene, glm::vec2 position)
    : pb::Entity(scene, 0)
{
    pb::BasicTransformComponent* transform = new pb::BasicTransformComponent(this);
    transform->SetPosition(glm::vec3(position.x, position.y, 0));
    
    pb::ModelComponent* model = new pb::ModelComponent(this,
                                                       pb::Engine::Instance()->GetModelRenderer()->GetModel("skybox"),
                                                       pb::Engine::Instance()->GetModelRenderer()->GetTexture("skybox"));
    
    glm::mat4x4 localTransform;
    localTransform = glm::rotate(localTransform, 90.f, glm::vec3(1,0,0));
    localTransform = glm::scale(localTransform, glm::vec3(4000, 4000, 4000));
    model->SetLocalTransform(localTransform);
    model->SetLayer(kGraphicLayerBackground);
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
