#include "pixelboost/logic/entity.h"

namespace pb
{
    class Message;
    class Scene;
}

class Asteroid : public pb::Entity
{
    PB_DECLARE_ENTITY
    
public:
    Asteroid(pb::Scene* scene, pb::Entity* parent, pb::DbEntity* creationEntity);
    ~Asteroid();
    
    void Initialise(glm::vec2 position, float scale);
    
public:
    void OnUpdate(const pb::Message& message);
    
private:
    float _Roll;
    float _Rotation;
    float _Scale;
    float _Speed;
};
