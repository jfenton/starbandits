#include "pixelboost/logic/entity.h"

class GameUi : public pb::Entity
{
public:
    GameUi(pb::Scene* scene);
    ~GameUi();
    
public:
    virtual pb::Uid GetType() const;
    static pb::Uid GetStaticType();
    
    void OnUpdate(const pb::Message& message);
};
