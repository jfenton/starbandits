#pragma once

#include "pixelboost/logic/entity.h"

namespace pb
{
    class Message;
    class Scene;
}

class BackgroundStars : public pb::Entity
{
public:
    BackgroundStars(pb::Scene* scene);
    ~BackgroundStars();
    
public:
    virtual pb::Uid GetType() const;
    static pb::Uid GetStaticType();
};
