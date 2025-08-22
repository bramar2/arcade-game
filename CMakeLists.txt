cmake_minimum_required(VERSION 3.27)
project(arcade-game)

cmake_policy(SET CMP0077 NEW)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_STANDARD 23)

if(BUILD_32BIT)
  set(CMAKE_CXX_FLAGS "-m32")
else()
  set(CMAKE_CXX_FLAGS "")
endif()

set(CMAKE_CXX_FLAGS_DEBUG "-g" CACHE STRING "" FORCE)
set(CMAKE_CXX_FLAGS_RELEASE "-O3" CACHE STRING "" FORCE)

if(NOT DEFINED BUILD_STATIC_APP)
  set(BUILD_STATIC_APP ON)
endif()

if(NOT BUILD_STATIC_APP)
  message("[arcade-game] Building application in DYNAMIC mode")
  find_package(SDL3 REQUIRED)
  find_package(SDL3_ttf REQUIRED)
  
  set(SDL_TARGET SDL3::SDL3)
  set(SDLTTF_TARGET SDL3_ttf::SDL3_ttf)
else()
  message("[arcade-game] Building application in STATIC mode")
  set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
  set(SDL_BUILD_SHARED OFF CACHE BOOL "" FORCE)
  set(CMAKE_POSITION_INDEPENDENT_CODE ON CACHE BOOL "" FORCE)

  set(SDLTTF_VENDORED ON CACHE BOOL "" FORCE)
  set(SDLTTF_FREETYPE ON CACHE BOOL "" FORCE)
  set(SDLTTF_BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

  add_subdirectory(external/SDL3 EXCLUDE_FROM_ALL)
  add_subdirectory(external/SDL3_ttf EXCLUDE_FROM_ALL)

  if(TARGET SDL3::SDL3-static)
    set(SDL_TARGET SDL3::SDL3-static)
  elseif(TARGET SDL3::SDL3)
    set(SDL_TARGET SDL3::SDL3)
  else()
    message(FATAL_ERROR "SDL3 target not found")
  endif()
  
  if(TARGET SDL3_ttf::SDL3_ttf-static)
    set(SDLTTF_TARGET SDL3_ttf::SDL3_ttf-static)
  elseif(TARGET SDL3_ttf::SDL3_ttf)
    set(SDLTTF_TARGET SDL3_ttf::SDL3_ttf)
  else()
    message(FATAL_ERROR "SDL3_ttf target not found")
  endif()

endif()

add_executable(arcade-game 
	program.cpp app_icon.rc

	json/json.hpp json/input.cpp json/input.hpp
	json/json_array.cpp json/json_array.hpp
	json/json_bool.cpp json/json_bool.hpp
	json/json_element.hpp
	json/json_null.cpp json/json_null.hpp
	json/json_number.cpp json/json_number.hpp
	json/json_object.cpp json/json_object.hpp
	json/json_string.cpp json/json_string.hpp
	json/json_type.hpp
	json/parse.cpp json/parse.hpp
	json/path.cpp json/path.hpp
	
	component/base.hpp
	component/main.hpp component/main.cpp
	component/game.hpp component/game.cpp
	component/rotate.hpp component/rotate.cpp
	component/scoring.hpp component/scoring.cpp
	component/gravity.hpp component/gravity.cpp
	component/grid_test.hpp

	text/f1.hpp text/f1.cpp
	text/f2.hpp text/f2.cpp

	logo/logo.hpp logo/logo.cpp
	logo/window.hpp logo/window.cpp

	cursor/cursor.hpp cursor/cursor.cpp

	blocks/red.hpp blocks/red.cpp
	blocks/blue.hpp blocks/blue.cpp
	blocks/turqoise.hpp blocks/turqoise.cpp
	blocks/green.hpp blocks/green.cpp
	blocks/orange.hpp blocks/orange.cpp
	blocks/purple.hpp blocks/purple.cpp
	blocks/yellow.hpp blocks/yellow.cpp

	sound/setup.hpp sound/setup.cpp
	sound/id.hpp sound/id.cpp
	sound/theme.hpp sound/theme.cpp
	sound/blockout.hpp sound/blockout.cpp
	sound/clear_four.hpp sound/clear_four.cpp
	sound/hard_drop.hpp sound/hard_drop.cpp
	sound/line_clear.hpp sound/line_clear.cpp
	sound/lock.hpp sound/lock.cpp
	sound/move.hpp sound/move.cpp
	sound/rotate.hpp sound/rotate.cpp

	name/name.hpp

	settings/icon.hpp settings/icon.cpp
	settings/settings.hpp settings/settings.cpp
)

if (WIN32)
  if (BUILD_STATIC_APP)
    target_link_options(arcade-game PRIVATE -mwindows -static -static-libgcc -static-libstdc++)
  else()
    target_link_options(arcade-game PRIVATE -mwindows -static-libgcc -static-libstdc++)
  endif()
endif()



target_link_libraries(arcade-game PRIVATE ${SDL_TARGET} ${SDLTTF_TARGET})