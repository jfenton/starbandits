#include "pixelboost/db/register.h"

#include "database/entities/turret.h"

TurretHomingDefinition::TurretHomingDefinition()
{
}

TurretLaserDefinition::TurretLaserDefinition()
{
}

PIXELBOOST_START_STRUCT(TurretHomingDefinition, "TurretHoming")
PIXELBOOST_END_STRUCT

PIXELBOOST_START_STRUCT(TurretLaserDefinition, "TurretLaser")
PIXELBOOST_END_STRUCT