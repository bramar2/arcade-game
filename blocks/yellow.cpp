#include "yellow.hpp"

namespace Block::Yellow {
	static constexpr unsigned char yellow_bmp[] = {};
  static constexpr unsigned int yellow_bmp_len = 3030;

  static constexpr unsigned char yellow_ghost_bmp[] = {};
  static constexpr unsigned int yellow_ghost_bmp_len = 3982;

  SDL_Surface* surface = nullptr;
  SDL_Texture* texture = nullptr;
  SDL_Surface* ghostSurface = nullptr;
  SDL_Texture* ghostTexture = nullptr;

  void init(SDL_Renderer* renderer) {
    surface = SDL_LoadBMP_IO(SDL_IOFromConstMem(yellow_bmp, yellow_bmp_len), true); 
    ghostSurface = SDL_LoadBMP_IO(SDL_IOFromConstMem(yellow_ghost_bmp, yellow_ghost_bmp_len), true); 

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