#include "pixelboost/logic/entity.h"

namespace pb
{
    class Message;
    class Scene;
}

class Projectile : public pb::Entity
{
public:
    Projectile(pb::Scene* scene, glm::vec3 position, float speed);
    ~Projectile();
    
public:
    virtual pb::Uid GetType() const;
    static pb::Uid GetStaticType();
    
    void OnUpdate(const pb::Message& message);
    
private:
    float _Life;
};
