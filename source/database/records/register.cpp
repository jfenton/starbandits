#include "pixelboost/db/register.h"

#include "database/records/register.h"

PB_DB_DECLARE_STRUCT(LevelSegment)

PB_DB_BEGIN_NAMESPACE(GameRecords)
    PB_DB_REGISTER_STRUCT(LevelSegment)
PB_DB_END_NAMESPACE
