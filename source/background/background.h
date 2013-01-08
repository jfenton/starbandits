#include "pixelboost/logic/entity.h"

namespace pb
{
    class Message;
    class Scene;
}

class BackgroundTile : public pb::Entity
{
public:
    BackgroundTile(pb::Scene* scene, glm::vec2 position);
    ~BackgroundTile();
    
public:
    virtual pb::Uid GetType() const;
    static pb::Uid GetStaticType();
    
private:
};