#pragma once

#include <SDL3/SDL.h>

namespace Sound {
  void init_theme();
  void start_theme();
  void tick_theme();
  void stop_theme();
  void set_volume_theme(float volume);
  void destroy_theme();
}