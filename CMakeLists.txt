cmake_minimum_required (VERSION 2.8.10)
project (doublewow)

include (pixelboost/build.cmake)

pixelboost_setup_pre(pixelboost doublewow "com.pixelballoon.doublewow" "The Double Wow Factor")

include_directories("source")

set (BACKGROUND_SOURCES
	"source/background/background.cpp"
	"source/background/background.h"
	"source/background/planet.cpp"
	"source/background/planet.h"
	"source/background/stars.cpp"
	"source/background/stars.h"
)

set (CORE_SOURCES
	"source/core/game.cpp"
	"source/core/game.h"
	)

set (DATABASE_SOURCES
	"source/database/entities/asteroid.cpp"
	"source/database/entities/asteroid.h"
	"source/database/entities/homingMine.cpp"
	"source/database/entities/homingMine.h"
	"source/database/entities/register.cpp"
	"source/database/entities/register.h"
	"source/database/entities/staticMine.cpp"
	"source/database/entities/staticMine.h"
	"source/database/entities/stealthBomber.cpp"
	"source/database/entities/stealthBomber.h"
	"source/database/entities/turret.cpp"
	"source/database/entities/turret.h"
	"source/database/records/levelSegment.cpp"
	"source/database/records/levelSegment.h"
	"source/database/records/register.cpp"
	"source/database/records/register.h"
)

set (ENEMY_SOURCES
	"source/enemies/asteroid.cpp"
	"source/enemies/asteroid.h"
	"source/enemies/homingMine.cpp"
	"source/enemies/homingMine.h"
	"source/enemies/staticMine.cpp"
	"source/enemies/staticMine.h"
	"source/enemies/stealthBomber.cpp"
	"source/enemies/stealthBomber.h"
	"source/enemies/turret.cpp"
	"source/enemies/turret.h"
	)

set (GAMEPLAY_SOURCES
	"source/gameplay/damage.cpp"
	"source/gameplay/damage.h"
	"source/gameplay/explosion.cpp"
	"source/gameplay/explosion.h"
	"source/gameplay/health.cpp"
	"source/gameplay/health.h"
)

set (LEVEL_SOURCES
	"source/level/levelSegment.cpp"
	"source/level/levelSegment.h"
	)

set (MENU_SOURCES
	"source/menu/menuItem.cpp"
	"source/menu/menuItem.h"
	)

set (PLAYER_WEAPON_SOURCES
	"source/player/weapon/laser.cpp"
	"source/player/weapon/laser.h"
	"source/player/weapon/missile.cpp"
	"source/player/weapon/missile.h"
	)

set (PLAYER_SOURCES
	"source/player/grapple.cpp"
	"source/player/grapple.h"
	"source/player/player.cpp"
	"source/player/player.h"
	"source/player/projectile.cpp"
	"source/player/projectile.h"
	)

set (SCREENS_SOURCES
	"source/screens/game.cpp"
	"source/screens/game.h"
	"source/screens/menu.cpp"
	"source/screens/menu.h"
	)

set (UI_SOURCES
	"source/ui/ui.cpp"
	"source/ui/ui.h"
	)

set (GAME_SOURCES
	${BACKGROUND_SOURCES}
	${CORE_SOURCES}
	${DATABASE_SOURCES}
	${ENEMY_SOURCES}
	${GAMEPLAY_SOURCES}
	${LEVEL_SOURCES}
	${MENU_SOURCES}
	${PLAYER_WEAPON_SOURCES}
	${PLAYER_SOURCES}
	${SCREENS_SOURCES}
	${UI_SOURCES}
	)

source_group (background FILES ${BACKGROUND_SOURCES})
source_group (core FILES ${CORE_SOURCES})
source_group (database FILES ${DATABASE_SOURCES})
source_group (enemies FILES ${ENEMY_SOURCES})
source_group (gameplay FILES ${GAMEPLAY_SOURCES})
source_group (level FILES ${LEVEL_SOURCES})
source_group (menu FILES ${MENU_SOURCES})
source_group (player\\weapon FILES ${PLAYER_WEAPON_SOURCES})
source_group (player FILES ${PLAYER_SOURCES})
source_group (screens FILES ${SCREENS_SOURCES})
source_group (ui FILES ${UI_SOURCES})

pixelboost_setup_post()
