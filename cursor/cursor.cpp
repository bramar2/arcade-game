#include "cursor.hpp"

namespace Cursor {

	SDL_Cursor* defaultCursor = nullptr;
	SDL_Cursor* pointerCursor = nullptr;

	void init() {
		defaultCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_DEFAULT);
		pointerCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_POINTER);
	}

	void destroy() {
		SDL_DestroyCursor(defaultCursor);
		SDL_DestroyCursor(pointerCursor);
	}
}