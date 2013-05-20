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
#include "pixelboost/logic/component/transform.h"
#include "pixelboost/logic/message/update.h"
#include "pixelboost/logic/system/graphics/render/render.h"
#include "pixelboost/logic/scene.h"

#include "background/background.h"
#include "common/layers.h"

PB_DEFINE_ENTITY(BackgroundTile)

BackgroundTile::BackgroundTile(pb::Scene* scene, pb::Entity* parent, pb::DbEntity* creationEntity)
    : pb::Entity(scene, parent, creationEntity)
{
    CreateComponent<pb::TransformComponent>();
    
    auto model = CreateComponent<pb::ModelComponent>();
    model->SetModel("/models/skybox");
    model->SetMaterial("/materials/skybox");
    
    glm::mat4x4 localTransform;
    localTransform = glm::rotate(localTransform, 90.f, glm::vec3(1,0,0));
    localTransform = glm::scale(localTransform, glm::vec3(4000, 4000, 4000));
    model->SetLocalTransform(localTransform);
    model->SetLayer(kGraphicLayerBackground);
}

BackgroundTile::~BackgroundTile()
{
    
}
