#include "pixelboost/db/register.h"

#include "database/records/levelSegment.h"

LevelSegmentDefinition::LevelSegmentDefinition()
{
}

PB_DB_BEGIN_STRUCT(LevelSegment)
    PB_DB_FIELD_FLOAT(Length)
PB_DB_END_STRUCT
