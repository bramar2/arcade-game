#pragma once

#include <SDL3/SDL_render.h>
#include <string>
#include <vector>

namespace Settings {

	extern bool vsync;
	extern bool holdAbility;
	extern bool ghostPiece;
	extern int nextPieceCnt;
	extern int musicVolume;
	extern int sfxVolume;
	extern std::vector<unsigned long long> highScores;

	void init(SDL_Renderer* renderer, const std::string& settingFile);
	void update();
  	void save();
  	void reset();
  	void destroy();
}