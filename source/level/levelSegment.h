#pragma once

#include "pixelboost/db/definitions.h"

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
    
    void Create();
    
    float GetLength();
    
private:
    const pb::DbRecord* _Record;
    pb::Scene* _Scene;
    float _Offset;
};
