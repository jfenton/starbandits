#include "pixelboost/db/register.h"

#include "database/entities/turret.h"

TurretHomingDefinition::TurretHomingDefinition()
{
}

TurretLaserDefinition::TurretLaserDefinition()
{
}

PB_DB_BEGIN_STRUCT(TurretHoming)
PB_DB_END_STRUCT

PB_DB_BEGIN_STRUCT(TurretLaser)
PB_DB_END_STRUCT
