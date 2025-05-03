#include "logo.hpp"

namespace Logo {
	static constexpr unsigned char T_logo_bmp[] = {};
  static constexpr unsigned int T_logo_bmp_len = 2776138;

  SDL_Surface* logoSurface = nullptr;
  SDL_Texture* logoTexture = nullptr;

  void init(SDL_Renderer* renderer) {
  	logoSurface = SDL_LoadBMP_IO(SDL_IOFromConstMem(T_logo_bmp, T_logo_bmp_len), true);
  	logoTexture = SDL_CreateTextureFromSurface(renderer, logoSurface);
  }
  void destroy() {
  	SDL_DestroySurface(logoSurface);
  	SDL_DestroyTexture(logoTexture);
  }
}