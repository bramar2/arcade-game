#include "turqoise.hpp"

namespace Block::Turqoise {
	static constexpr unsigned char turqoise_bmp[] = {};
  static constexpr unsigned int turqoise_bmp_len = 2814;

  static constexpr unsigned char turqoise_ghost_bmp[] = {};
  static constexpr unsigned int turqoise_ghost_bmp_len = 3982;

  SDL_Surface* surface = nullptr;
  SDL_Texture* texture = nullptr;
  SDL_Surface* ghostSurface = nullptr;
  SDL_Texture* ghostTexture = nullptr;

  void init(SDL_Renderer* renderer) {
    surface = SDL_LoadBMP_IO(SDL_IOFromConstMem(turqoise_bmp, turqoise_bmp_len), true); 
    ghostSurface = SDL_LoadBMP_IO(SDL_IOFromConstMem(turqoise_ghost_bmp, turqoise_ghost_bmp_len), true); 

    texture = SDL_CreateTextureFromSurface(renderer, surface);
    ghostTexture = SDL_CreateTextureFromSurface(renderer, ghostSurface);
  }

  void destroy() {
    SDL_DestroySurface(surface);
    SDL_DestroySurface(ghostSurface);

    SDL_DestroyTexture(texture);
    SDL_DestroyTexture(ghostTexture);
  }
}