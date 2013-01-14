#include "pixelboost/db/register.h"

#include "database/entities/register.h"

PIXELBOOST_DECLARE_CLASS(AsteroidDefinition);
PIXELBOOST_DECLARE_CLASS(HomingMineDefinition);
PIXELBOOST_DECLARE_CLASS(StaticMineDefinition);
PIXELBOOST_DECLARE_CLASS(StealthBomberDefinition);
PIXELBOOST_DECLARE_CLASS(TurretDefinition);

PIXELBOOST_START_REGISTER(RegisterEntityCreation)
    PIXELBOOST_REGISTER(AsteroidDefinition)
    PIXELBOOST_REGISTER(HomingMineDefinition)
    PIXELBOOST_REGISTER(StaticMineDefinition)
    PIXELBOOST_REGISTER(StealthBomberDefinition)
    PIXELBOOST_REGISTER(TurretDefinition)
PIXELBOOST_END_REGISTER
