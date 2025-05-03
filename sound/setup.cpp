#include "setup.hpp"

#include "id.hpp"
#include "theme.hpp"
#include "blockout.hpp"
#include "clear_four.hpp"
#include "hard_drop.hpp"
#include "line_clear.hpp"
#include "lock.hpp"
#include "move.hpp"
#include "rotate.hpp"

namespace Sound {
	bool init() {
		if(!(audioDeviceId = SDL_OpenAudioDevice(Sound::audioDeviceId, NULL))) {
	    	return false;
	    }
		init_theme();

		init_blockout();
		init_clear_four();
		init_hard_drop();
		init_line_clear();
		init_lock();
		init_move();
		init_rotate();

		return true;
	}
	void destroy() {
		destroy_theme();

		destroy_blockout();
		destroy_clear_four();
		destroy_hard_drop();
		destroy_line_clear();
		destroy_lock();
		destroy_move();
		destroy_rotate();

		SDL_CloseAudioDevice(audioDeviceId);
	}
}