#pragma once

#include <vector>

#include "pixelboost/db/record.h"
#include "pixelboost/db/register.h"

namespace pb
{
    class DbRecord;
    class Scene;
}

class LevelSegment
{
public:
    LevelSegment(pb::Scene* scene);
    ~LevelSegment();
    
    pb::Uid Create();
    
    float GetLength();
    
private:
    pb::DbRecord::EntityMap::const_iterator _EntityIt;
    
    const pb::DbRecord* _Record;
    pb::Scene* _Scene;
};
