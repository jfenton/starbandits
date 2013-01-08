cmake_minimum_required (VERSION 2.8.10)
project (doublewow)

include (pixelboost/build.cmake)

pixelboost_setup_pre(pixelboost doublewow "com.pixelballoon.doublewow" "The Double Wow Factor")

include_directories("source")

set (GAME_SOURCES
	"source/background/background.cpp"
	"source/background/background.h"
	"source/game/game.cpp"
	"source/game/game.h"
	"source/player/ship.cpp"
	"source/player/ship.h"
	"source/screens/game.cpp"
	"source/screens/game.h"
	)

pixelboost_setup_post()
