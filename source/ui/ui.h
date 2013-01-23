#include "pixelboost/logic/entity.h"

class GameUi : public pb::Entity
{
public:
    GameUi(pb::Scene* scene, pb::Uid playerId, int index);
    ~GameUi();
    
public:
    virtual pb::Uid GetType() const;
    static pb::Uid GetStaticType();
    
    void OnUpdate(const pb::Message& message);

private:
    pb::Uid _PlayerId;
};
