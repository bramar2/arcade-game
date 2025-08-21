#pragma once

#include "SDL3/SDL_keycode.h"
#include "base.hpp"

#include <SDL3/SDL_render.h>
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <map>
#include <string>

namespace Ui {
	class Main : public Base {
	private:
		const std::string CREDITS = R"(Credits & Attribution:
SDL - library for audio and display
Nintendo - idea, sprites, music, sfx
UXWing - icons
CodeMan38 - arcade font "Press Start 2P"
GNU - font "FreeMono"
bramar2 - ui/ux, code, data,
              logic, game mechanics)";
		const std::string GITHUB_LINK = "https://github.com/bramar2/arcade-game";
		const int w = 840, h = 840;
		const int overlayAlpha = 128;
		const int settingHoverOverlayAlpha = 100;
		const float settingMargin = 64;
		const int fullOpenSettingsTime = 350;

		SDL_Window* window;
		SDL_Renderer* renderer;
		TTF_TextEngine* textEngine;
		SwitchFunc funcSwitchUi;

		TTF_Text* playText;
		TTF_Text* hsText;
		TTF_Text* noneText;
		TTF_Text* exitText;
		TTF_Text* creditsText;
		TTF_Text* linkText;


		float logoWidth, logoHeight;
		int tw, th, tx, ty;
		int hsW;
		int creditsHeight;
		int linkWidth, linkHeight;

		SDL_FRect settingBtnRect;
		SDL_FRect logoPos;

		bool settingsUi = false;

		Uint64 openTime;
		Uint64 closeTime;
		bool closing = false;

		std::map<std::string, TTF_Text*> settingsText;
		std::map<std::string, int> textWidth;
	public:
		inline int id() override { return 0; }
		void init(SDL_Window* window, SDL_Renderer* renderer, TTF_TextEngine* textEngine, const SwitchFunc& funcSwitchUi) override;
		void render() override;
		void mousedown(Uint8 button) override;
		void keydown(SDL_Keycode keycode) override;
		void destroy() override;

		TTF_Text* setting_text(const std::string& str, int r = 160, int g = 160, int b = 160);
	};
}