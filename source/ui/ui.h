#include "pixelboost/logic/entity.h"

namespace pb
{
    class FontComponent;
}

class GameUi : public pb::Entity
{
    PB_DECLARE_ENTITY
    
public:
    GameUi(pb::Scene* scene, pb::Entity* entity, pb::DbEntity* creationEntity);
    ~GameUi();
    
    void Initialise(pb::Uid playerId, int index);
    
public:
    void OnUpdate(const pb::Message& message);

private:
    pb::FontComponent* _Score;
    
    pb::Uid _PlayerId;
};
