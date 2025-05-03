#include "orange.hpp"

namespace Block::Orange {
	static constexpr unsigned char orange_bmp[] = {};
  static constexpr unsigned int orange_bmp_len = 2814;

  static constexpr unsigned char orange_ghost_bmp[] = {};
  static constexpr unsigned int orange_ghost_bmp_len = 3982;

  SDL_Surface* surface = nullptr;
  SDL_Texture* texture = nullptr;
  SDL_Surface* ghostSurface = nullptr;
  SDL_Texture* ghostTexture = nullptr;

  void init(SDL_Renderer* renderer) {
    surface = SDL_LoadBMP_IO(SDL_IOFromConstMem(orange_bmp, orange_bmp_len), true); 
    ghostSurface = SDL_LoadBMP_IO(SDL_IOFromConstMem(orange_ghost_bmp, orange_ghost_bmp_len), true); 

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