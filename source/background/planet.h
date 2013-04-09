#include "pixelboost/logic/entity.h"

class Planet : public pb::Entity
{
    PB_DECLARE_ENTITY
    
public:
    Planet(pb::Scene* scene, pb::Entity* parent, pb::DbEntity* creationEntity);
    ~Planet();
    
    void Initialise(const std::string& name, glm::vec3 position, float size);
    
public:
    void OnUpdate(const pb::Message& message);
};
