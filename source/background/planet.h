#include "pixelboost/logic/entity.h"

class Planet : public pb::Entity
{
public:
    Planet(pb::Scene* scene, const std::string& name, glm::vec3 position, float size);
    ~Planet();
    
public:
    virtual pb::Uid GetType() const;
    static pb::Uid GetStaticType();
    
    void OnUpdate(const pb::Message& message);
};
