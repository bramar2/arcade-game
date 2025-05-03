#pragma once

#include <SDL3/SDL_render.h>

namespace Settings {
	extern SDL_Surface* iconSurface;
  	extern SDL_Surface* iconHoveredSurface;
  	
  	extern SDL_Texture* iconTexture;
  	extern SDL_Texture* iconHoveredTexture;


  	void init_icon(SDL_Renderer* renderer);
  	void destroy_icon();
}