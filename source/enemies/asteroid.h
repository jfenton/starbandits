#include "pixelboost/logic/entity.h"

namespace pb
{
    class Message;
    class Scene;
}

class Asteroid : public pb::Entity
{
public:
    Asteroid(pb::Scene* scene, glm::vec2 position, float scale);
    ~Asteroid();
    
public:
    virtual pb::Uid GetType() const;
    static pb::Uid GetStaticType();
    
    void OnUpdate(const pb::Message& message);
    
private:
    float _Roll;
    float _Rotation;
    float _Scale;
    float _Speed;
};
