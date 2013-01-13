#include "pixelboost/db/register.h"

#include "database/records/levelSegment.h"

LevelSegmentDefinition::LevelSegmentDefinition()
{
}

PIXELBOOST_START_STRUCT(LevelSegmentDefinition, "LevelSegment")
    PIXELBOOST_FIELD_FLOAT(Length, "Length")
PIXELBOOST_END_STRUCT
