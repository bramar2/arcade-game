cmake_minimum_required(VERSION 3.27)
project(arcade-game)

set(CMAKE_CXX_STANDARD 23)

find_package(SDL3 REQUIRED)
find_package(SDL3_ttf REQUIRED)

add_executable(arcade-game
	program.cpp

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

target_link_libraries(arcade-game SDL3::SDL3 SDL3_ttf::SDL3_ttf)