#include "pixelboost/db/register.h"

#include "database/entities/register.h"

PIXELBOOST_DECLARE_CLASS(AsteroidDefinition);
PIXELBOOST_DECLARE_CLASS(HomingMineDefinition);
PIXELBOOST_DECLARE_CLASS(StaticMineDefinition);
PIXELBOOST_DECLARE_CLASS(StealthBomberDefinition);
PIXELBOOST_DECLARE_CLASS(TurretHomingDefinition);
PIXELBOOST_DECLARE_CLASS(TurretLaserDefinition);

PIXELBOOST_START_REGISTER(RegisterEntityCreation)
    PIXELBOOST_REGISTER(AsteroidDefinition)
    PIXELBOOST_REGISTER(HomingMineDefinition)
    PIXELBOOST_REGISTER(StaticMineDefinition)
    PIXELBOOST_REGISTER(StealthBomberDefinition)
    PIXELBOOST_REGISTER(TurretHomingDefinition)
    PIXELBOOST_REGISTER(TurretLaserDefinition)
PIXELBOOST_END_REGISTER
