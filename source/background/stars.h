#pragma once

#include "pixelboost/logic/entity.h"

namespace pb
{
    class Message;
    class Scene;
}

class BackgroundStars : public pb::Entity
{
    PB_DECLARE_ENTITY
    
public:
    BackgroundStars(pb::Scene* scene, pb::Entity* parent, pb::DbEntity* creationEntity);
    ~BackgroundStars();
};
