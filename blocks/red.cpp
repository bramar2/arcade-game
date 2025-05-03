#include "red.hpp"

namespace Block::Red {
	static constexpr unsigned char red_bmp[] = {};
  static constexpr unsigned int red_bmp_len = 3030;

  static constexpr unsigned char red_ghost_bmp[] = {};
  static constexpr unsigned int red_ghost_bmp_len = 3982;

  SDL_Surface* surface = nullptr;
  SDL_Texture* texture = nullptr;
  SDL_Surface* ghostSurface = nullptr;
  SDL_Texture* ghostTexture = nullptr;

  void init(SDL_Renderer* renderer) {
    surface = SDL_LoadBMP_IO(SDL_IOFromConstMem(red_bmp, red_bmp_len), true); 
    ghostSurface = SDL_LoadBMP_IO(SDL_IOFromConstMem(red_ghost_bmp, red_ghost_bmp_len), true); 

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