#pragma once

#include <SDL3/SDL_render.h>
namespace Block::Blue {
  extern SDL_Surface* surface;
  extern SDL_Texture* texture;
  extern SDL_Surface* ghostSurface;
  extern SDL_Texture* ghostTexture;

  void init(SDL_Renderer* renderer);
  void destroy();
}