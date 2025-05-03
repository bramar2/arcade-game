#pragma once

#include <SDL3/SDL_mouse.h>
namespace Cursor {
	extern SDL_Cursor* defaultCursor;
	extern SDL_Cursor* pointerCursor;

	void init();
	void destroy();
}