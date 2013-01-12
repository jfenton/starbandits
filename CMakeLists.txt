cmake_minimum_required (VERSION 2.8.10)
project (doublewow)

include (pixelboost/build.cmake)

pixelboost_setup_pre(pixelboost doublewow "com.pixelballoon.doublewow" "The Double Wow Factor")

include_directories("source")

set (GAME_SOURCES
	"source/background/background.cpp"
	"source/background/background.h"
	"source/background/planet.cpp"
	"source/background/planet.h"
	"source/enemies/asteroid.cpp"
	"source/enemies/asteroid.h"
	"source/enemies/homingMine.cpp"
	"source/enemies/homingMine.h"
	"source/enemies/staticMine.cpp"
	"source/enemies/staticMine.h"
	"source/gameplay/damage.cpp"
	"source/gameplay/damage.h"
	"source/gameplay/explosion.cpp"
	"source/gameplay/explosion.h"
	"source/gameplay/health.cpp"
	"source/gameplay/health.h"
	"source/game/game.cpp"
	"source/game/game.h"
	"source/player/grapple.cpp"
	"source/player/grapple.h"
	"source/player/grappleComponent.cpp"
	"source/player/grappleComponent.h"
    "source/player/player.cpp"
	"source/player/player.h"
	"source/player/projectile.cpp"
	"source/player/projectile.h"
	"source/screens/game.cpp"
	"source/screens/game.h"
	)

pixelboost_setup_post()
