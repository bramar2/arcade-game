#include "window.hpp"

#include <SDL3/SDL_render.h>
#include <SDL3/SDL_surface.h>

namespace Logo {
	static constexpr unsigned char T_window_bmp[] = {};
  static constexpr unsigned int T_window_bmp_len = 1548138;

  static SDL_Surface* original = nullptr;
  SDL_Surface* windowSurface = nullptr;

  void init_window() {
  	original = SDL_LoadBMP_IO(SDL_IOFromConstMem(T_window_bmp, T_window_bmp_len), true);

  	int size = SDL_max(original->w, original->h);
    windowSurface = SDL_CreateSurface(size, size, SDL_PIXELFORMAT_RGBA32);

    SDL_FillSurfaceRect(windowSurface, NULL, SDL_MapRGBA(SDL_GetPixelFormatDetails(SDL_PIXELFORMAT_RGBA32), NULL, 0, 0, 0, 0));

    SDL_Rect dest;
    dest.x = (size - original->w) / 2;
    dest.y = (size - original->h) / 2;
    dest.w = original->w;
    dest.h = original->h;

    SDL_BlitSurface(original, NULL, windowSurface, &dest);
  }
  void destroy_window() {
  	SDL_DestroySurface(windowSurface);
  	SDL_DestroySurface(original);
  }
}