cmake_minimum_required (VERSION 2.8.10)
project (doublewow)

include (pixelboost/build.cmake)

pixelboost_setup_pre(pixelboost doublewow "com.pixelballoon.doublewow" "The Double Wow Factor")

include_directories("source")

set (GAME_SOURCES
	"source/game/game.cpp"
	)

pixelboost_setup_post()
