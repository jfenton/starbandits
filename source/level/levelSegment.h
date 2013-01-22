#pragma once

#include <vector>

#include "pixelboost/db/definitions.h"
#include "pixelboost/db/record.h"

namespace pb
{
    class DbRecord;
    class Scene;
}

class LevelSegment
{
public:
    LevelSegment(pb::Scene* scene, float offset);
    ~LevelSegment();
    
    pb::Uid Create();
    
    float GetLength();
    
private:
    pb::DbRecord::EntityMap::const_iterator _EntityIt;
    
    const pb::DbRecord* _Record;
    pb::Scene* _Scene;
    float _Offset;
};
