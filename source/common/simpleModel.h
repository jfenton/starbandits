#pragma once

#include "pixelboost/logic/entity.h"

class SimpleModel : public pb::Entity
{
    PB_DECLARE_ENTITY
    
public:
    SimpleModel(pb::Scene* scene, pb::Entity* parent, pb::DbEntity* creationEntity);
    virtual ~SimpleModel();
    
    SimpleModel* Initialise(const std::string& model, glm::vec3 position = glm::vec3(0,0,0));
};