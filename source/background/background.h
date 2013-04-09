#pragma once

#include "pixelboost/logic/entity.h"

namespace pb
{
    class Message;
    class Scene;
    class SpriteRenderable;
}

class BackgroundTile : public pb::Entity
{
    PB_DECLARE_ENTITY
    
public:
    BackgroundTile(pb::Scene* scene, pb::Entity* parent, pb::DbEntity* creationEntity);
    ~BackgroundTile();
};
