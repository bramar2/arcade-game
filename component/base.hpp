#pragma once

#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <functional>

namespace Ui {
	using SwitchFunc = std::function<void(int)>;
	class Base {
	public:
		Base() {};
		virtual int id() = 0;
		virtual void init(SDL_Window* window, SDL_Renderer* renderer, TTF_TextEngine* textEngine, const SwitchFunc& funcSwitchUi) {};
		virtual void open() {}
		virtual void close() {}
		virtual void render() {}
		virtual void mouseup(Uint8 button) {}
		virtual void mousedown(Uint8 button) {}
		virtual void keydown(SDL_Keycode key) {}
		virtual void keyup(SDL_Keycode key) {}
		virtual void destroy() {}
		virtual ~Base() { destroy(); }
	};
}