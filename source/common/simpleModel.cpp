#include "pixelboost/logic/component/graphics/model.h"
#include "pixelboost/logic/component/transform.h"

#include "common/simpleModel.h"

PB_DEFINE_ENTITY(SimpleModel)

SimpleModel::SimpleModel(pb::Scene* scene, pb::Entity* parent, pb::DbEntity* creationEntity)
    : pb::Entity(scene, parent, creationEntity)
{
    CreateComponent<pb::TransformComponent>();
    CreateComponent<pb::ModelComponent>();
}

SimpleModel::~SimpleModel()
{
    
}

SimpleModel* SimpleModel::Initialise(const std::string& model, glm::vec3 position)
{
    GetComponent<pb::TransformComponent>()->SetPosition(position);
    GetComponent<pb::ModelComponent>()->SetModel(model);
    return this;
}
