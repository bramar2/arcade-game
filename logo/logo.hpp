#pragma once

#include <SDL3/SDL_render.h>

namespace Logo {
  extern SDL_Surface* logoSurface;
  extern SDL_Texture* logoTexture;

  void init(SDL_Renderer* renderer);
  void destroy();
}