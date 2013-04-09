#include "pixelboost/db/register.h"

#include "database/entities/register.h"

PB_DB_DECLARE_STRUCT(Asteroid);
PB_DB_DECLARE_STRUCT(HomingMine);
PB_DB_DECLARE_STRUCT(StaticMine);
PB_DB_DECLARE_STRUCT(StealthBomber);
PB_DB_DECLARE_STRUCT(TurretHoming);
PB_DB_DECLARE_STRUCT(TurretLaser);

PB_DB_BEGIN_NAMESPACE(GameEntities)
    PB_DB_REGISTER_STRUCT(Asteroid)
    PB_DB_REGISTER_STRUCT(HomingMine)
    PB_DB_REGISTER_STRUCT(StaticMine)
    PB_DB_REGISTER_STRUCT(StealthBomber)
    PB_DB_REGISTER_STRUCT(TurretHoming)
    PB_DB_REGISTER_STRUCT(TurretLaser)
PB_DB_END_NAMESPACE
