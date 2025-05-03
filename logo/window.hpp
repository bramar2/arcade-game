#pragma once

#include <SDL3/SDL_surface.h>
namespace Logo {
  extern SDL_Surface* windowSurface;

  void init_window();
  void destroy_window();
}