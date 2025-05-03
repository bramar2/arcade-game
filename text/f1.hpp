#pragma once

#include <SDL3_ttf/SDL_ttf.h>
namespace Font1 {
  extern TTF_Font* sz18;
  extern TTF_Font* sz24;
  extern TTF_Font* sz48;

  void init();
  void destroy();
};