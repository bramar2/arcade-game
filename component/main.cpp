#include "main.hpp"

#include "game.hpp"
#include "../logo/logo.hpp"
#include "../text/f1.hpp"
#include "../cursor/cursor.hpp"
#include "../settings/icon.hpp"
#include "../settings/settings.hpp"

#include <SDL3/SDL_render.h>
#include <SDL3_ttf/SDL_ttf.h>

namespace Ui {

	static int textHeight;

	void Main::init(SDL_Window* i_window, SDL_Renderer* i_renderer, TTF_TextEngine* i_textEngine, const SwitchFunc& i_funcSwitchUi) {
		window = i_window;
		renderer = i_renderer;
		textEngine = i_textEngine;
		funcSwitchUi = i_funcSwitchUi;

		playText = TTF_CreateText(textEngine, Font1::sz48, "PLAY", 0);
		hsText = TTF_CreateText(textEngine, Font1::sz18, "HIGH SCORE", 0);
		noneText = TTF_CreateText(textEngine, Font1::sz18, "None.", 0);

		SDL_GetTextureSize(Logo::logoTexture, &logoWidth, &logoHeight);

		logoPos = {(float) w/4, 80, (float) w / 2, logoHeight * w/2 / logoWidth};

		TTF_GetStringSize(Font1::sz18, "HIGH SCORE", 0, &hsW, &th);
		TTF_GetStringSize(Font1::sz48, "PLAY", 0, &tw, &th);

		tx = (w - tw) / 2.0;
		ty = logoPos.h + logoPos.y + 50;

		settingBtnRect = {
			.x = (float) tx + tw + 16,
			.y = (float) ty + (th - 36) / 2.0f,
			.w = 36,
			.h = 36
		};
	}

	TTF_Text* Main::setting_text(const std::string& str,
		int r, int g, int b) {
		if(!settingsText.count(str)) {
			settingsText[str] = TTF_CreateText(textEngine, Font1::sz18, str.data(), 0);
			int tw, th;
			TTF_GetStringSize(Font1::sz18, str.data(), 0, &tw, &th);
			textWidth[str] = tw;
			textHeight = th;
		}
		TTF_SetTextColor(settingsText[str], r, g, b, 255);
		return settingsText[str];
	}

	void Main::render() {
		SDL_RenderTexture(renderer, Logo::logoTexture, NULL, &logoPos);
		
		float mouseX, mouseY;
		SDL_GetMouseState(&mouseX, &mouseY);

		int r = 255, g = 255, b = 255;

		bool pointer = false;

		if(!settingsUi && tx <= mouseX && mouseX <= tx + tw &&
			ty <= mouseY && mouseY <= ty + th) {
			r -= 90, g -= 90, b -= 90;
			pointer = true;
		}
		
		TTF_SetTextColor(playText, r, g, b, 255);
		TTF_DrawRendererText(playText, tx, ty);

		if(!settingsUi &&
			settingBtnRect.x <= mouseX && mouseX <= settingBtnRect.x + settingBtnRect.w &&
			settingBtnRect.y <= mouseY && mouseY <= settingBtnRect.y + settingBtnRect.h) {
			SDL_RenderTexture(renderer, Settings::iconHoveredTexture, NULL, &settingBtnRect);	
			pointer = true;
		}else {
			SDL_RenderTexture(renderer, Settings::iconTexture, NULL, &settingBtnRect);
		}

		{ // high scores
			float hsX = 32, hsY = ty + th + 64;
			float hsDist = 40;
			TTF_DrawRendererText(hsText, hsX + 32, hsY);

			SDL_FRect hsRect = {
				hsX - 16, hsY - 16,
				(float) 16 + 32 + hsW + 32 + 16, 6 * hsDist + 16
			};

			SDL_SetRenderDrawColor(renderer, 180, 180, 180, 255);
			SDL_RenderRect(renderer, &hsRect);

			hsY += hsDist;
			if(Settings::highScores.empty()) {
				TTF_DrawRendererText(noneText, hsX, hsY);
			}else {
				int idx = 0;
				for(int idx = 0; idx < (int) Settings::highScores.size(); idx++) {
					std::string str = std::to_string(Settings::highScores[idx]);
					TTF_Text* text = TTF_CreateText(textEngine, Font1::sz18, str.data(), 0);
					if(idx == 0) {
						TTF_SetTextColor(text, 0, 215, 200, 255);
					}else if(idx == 1) {
						TTF_SetTextColor(text, 0, 205, 220, 255);
					}else if(idx == 2) {
						TTF_SetTextColor(text, 0, 180, 200, 255);
					}else if(idx == 3) {
						TTF_SetTextColor(text, 0, 130, 170, 255);
					}else {
						TTF_SetTextColor(text, 0, 65, 150, 255);
					}
					TTF_DrawRendererText(text, hsX, hsY);
					TTF_DestroyText(text);
					hsY += hsDist;
				}
			}
		}


		if(closing && SDL_GetTicks() - closeTime >= fullOpenSettingsTime) {
			closing = false;
			settingsUi = false;
		}

		if(settingsUi) {
			// gray overlay
			SDL_BlendMode blendMode;
			SDL_GetRenderDrawBlendMode(renderer, &blendMode);
			SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
			SDL_SetRenderDrawColor(renderer, 0, 0, 0, overlayAlpha);
			SDL_FRect rect {0, 0, (float) w, (float) h};
			SDL_RenderFillRect(renderer, &rect);

			int elapsed = (closing) ? (fullOpenSettingsTime - (SDL_GetTicks() - closeTime)) : (SDL_GetTicks() - openTime);
			float part = (float) std::min<int>(elapsed, fullOpenSettingsTime) / fullOpenSettingsTime;
			float mid = h / 2.0f;
			float workingH = h - 2.0f * settingMargin;

			rect = {settingMargin, mid - part * workingH / 2.0f, w - 2.0f * settingMargin, part * workingH};
			SDL_SetRenderDrawColor(renderer, 80, 80, 80, 150);
			SDL_RenderFillRect(renderer, &rect);

			SDL_SetRenderDrawBlendMode(renderer, blendMode);

			SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255);
			SDL_RenderRect(renderer, &rect);



			// Settings:
			// VSYNC
			// SHOW GHOST PIECE
			// HOLD PIECE
			// SHOW NEXT PIECES
			// MUSIC VOLUME
			// SFX VOLUME
			// CLEAR HIGH SCORES (BTN)

			float x = rect.x + 32;

			float backY = rect.y + 32;
			float resetY = backY + 32;
			float vsyncY = resetY + 128;

			const float dist = 36;
			float hpY = vsyncY + dist;
			float sgpY = hpY + dist;
			float npsY = sgpY + dist;
			float mvY = npsY + dist;
			float sfxvY = mvY + dist;
			float hsY = sfxvY + 2 * dist;

			do {
				if(backY + textHeight > rect.y + rect.h) break;

				if(x <= mouseX && mouseX <= x + textWidth["BACK"] &&
					backY <= mouseY && mouseY <= backY + textHeight) {
					pointer = true;
					TTF_DrawRendererText(setting_text("BACK", 165, 165, 165), x, backY);
				}else {
					TTF_DrawRendererText(setting_text("BACK", 255, 255, 255), x, backY);
				}

				if(resetY + textHeight > rect.y + rect.h) break;

				if(x <= mouseX && mouseX <= x + textWidth["RESET"] &&
					resetY <= mouseY && mouseY <= resetY + textHeight) {
					pointer = true;
					TTF_DrawRendererText(setting_text("RESET", 165, 165, 165), x, resetY);
				}else {
					TTF_DrawRendererText(setting_text("RESET", 255, 255, 255), x, resetY);
				}


				if(vsyncY + textHeight > rect.y + rect.h) break;
				TTF_DrawRendererText(setting_text("VSync"), x, vsyncY);
				if(hpY + textHeight > rect.y + rect.h) break;
				TTF_DrawRendererText(setting_text("Hold Ability"), x, hpY);
				if(sgpY + textHeight > rect.y + rect.h) break;
				TTF_DrawRendererText(setting_text("Show Ghost Piece"), x, sgpY);
				if(npsY + textHeight > rect.y + rect.h) break;
				TTF_DrawRendererText(setting_text("Next Piece Show Count"), x, npsY);
				if(mvY + textHeight > rect.y + rect.h) break;
				TTF_DrawRendererText(setting_text("Music Volume"), x, mvY);
				if(sfxvY + textHeight > rect.y + rect.h) break;
				TTF_DrawRendererText(setting_text("SFX Volume"), x, sfxvY);

				if(hsY + textHeight > rect.y + rect.h) break;
				if(x <= mouseX && mouseX <= x + textWidth["CLEAR HIGH SCORES"] &&
					hsY <= mouseY && mouseY <= hsY + textHeight) {
					pointer = true;
					TTF_DrawRendererText(setting_text("CLEAR HIGH SCORES", 165, 165, 165), x, hsY);
				}else {
					TTF_DrawRendererText(setting_text("CLEAR HIGH SCORES", 255, 255, 255), x, hsY);
				}
			}while(false);

			x += 32;
			static constexpr std::array<std::string, 2> boolText = {"Off", "On"};

			do {
				if(vsyncY + textHeight > rect.y + rect.h) break;
				int offset = 0;

				if(x + textWidth["VSync"] <= mouseX && mouseX <= x + textWidth["VSync"] + textWidth[boolText[Settings::vsync]] &&
					vsyncY <= mouseY && mouseY <= vsyncY + textHeight) {
					pointer = true, offset = 90;
				}else offset = 0;

				if(Settings::vsync) {
					TTF_DrawRendererText(setting_text("On", 0, 255 - offset, 0), x + textWidth["VSync"], vsyncY);
				}else {
					TTF_DrawRendererText(setting_text("Off", 255 - offset, 0, 0), x + textWidth["VSync"], vsyncY);
				}

				if(hpY + textHeight > rect.y + rect.h) break;

				if(x + textWidth["Hold Ability"] <= mouseX && mouseX <= x + textWidth["Hold Ability"] + textWidth[boolText[Settings::holdAbility]] &&
					hpY <= mouseY && mouseY <= hpY + textHeight) {
					pointer = true, offset = 90;
				}else offset = 0;

				if(Settings::holdAbility) {
					TTF_DrawRendererText(setting_text("On", 0, 255 - offset, 0), x + textWidth["Hold Ability"], hpY);
				}else {
					TTF_DrawRendererText(setting_text("Off", 255 - offset, 0, 0), x + textWidth["Hold Ability"], hpY);
				}


				if(sgpY + textHeight > rect.y + rect.h) break;

				if(x + textWidth["Show Ghost Piece"] <= mouseX && mouseX <= x + textWidth["Show Ghost Piece"] + textWidth[boolText[Settings::ghostPiece]] &&
					sgpY <= mouseY && mouseY <= sgpY + textHeight) {
					pointer = true, offset = 90;
				}else offset = 0;

				if(Settings::ghostPiece) {
					TTF_DrawRendererText(setting_text("On", 0, 255 - offset, 0), x + textWidth["Show Ghost Piece"], sgpY);
				}else {
					TTF_DrawRendererText(setting_text("Off", 255 - offset, 0, 0), x + textWidth["Show Ghost Piece"], sgpY);
				}


				if(npsY + textHeight > rect.y + rect.h) break;

				std::string npsStr = std::to_string(Settings::nextPieceCnt);

				if(x + textWidth["Next Piece Show Count"] <= mouseX &&
					mouseX <= x + textWidth["Next Piece Show Count"] + textWidth["-"]
					&& npsY <= mouseY && mouseY <= npsY + textHeight) {
					pointer = true, offset = 90;
				}else offset = 0;

				TTF_DrawRendererText(setting_text("-", 200 - offset, 0, 0), x + textWidth["Next Piece Show Count"], npsY);

				TTF_DrawRendererText(setting_text(npsStr, 255, 255, 255),
					x + textWidth["Next Piece Show Count"] + 64 + textWidth["-"], npsY);

				if(x + textWidth["Next Piece Show Count"] + 128 + textWidth[npsStr] + textWidth["-"] <= mouseX &&
					mouseX <= x + textWidth["Next Piece Show Count"] + 128 + textWidth[npsStr] + textWidth["-"] + textWidth["+"]
					&& npsY <= mouseY && mouseY <= npsY + textHeight) {
					pointer = true, offset = 90;
				}else offset = 0;

				TTF_DrawRendererText(setting_text("+", 0, 200 - offset, 0), x + textWidth["Next Piece Show Count"] + 128 + textWidth[npsStr] + textWidth["-"], npsY);


				if(mvY + textHeight > rect.y + rect.h) break;

				std::string mvStr = std::to_string(Settings::musicVolume) + "%";
				if(x + textWidth["Music Volume"] <= mouseX &&
					mouseX <= x + textWidth["Music Volume"] + textWidth["-"]
					&& mvY <= mouseY && mouseY <= mvY + textHeight) {
					pointer = true, offset = 90;
				}else offset = 0;
				TTF_DrawRendererText(setting_text("-", 200 - offset, 0, 0), x + textWidth["Music Volume"], mvY);

				TTF_DrawRendererText(setting_text(mvStr, 255, 255, 255),
					x + textWidth["Music Volume"] + 64 + textWidth["-"], mvY);

				if(x + textWidth["Music Volume"] + 128 + textWidth[mvStr] + textWidth["-"] <= mouseX &&
					mouseX <= x + textWidth["Music Volume"] + 128 + textWidth[mvStr] + textWidth["-"] + textWidth["+"]
					&& mvY <= mouseY && mouseY <= mvY + textHeight) {
					pointer = true, offset = 90;
				}else offset = 0;

				TTF_DrawRendererText(setting_text("+", 0, 200 - offset, 0), x + textWidth["Music Volume"] + 128 + textWidth[mvStr] + textWidth["-"], mvY);



				if(sfxvY + textHeight > rect.y + rect.h) break;

				std::string sfxvStr = std::to_string(Settings::sfxVolume) + "%";

				if(x + textWidth["SFX Volume"] <= mouseX &&
					mouseX <= x + textWidth["SFX Volume"] + textWidth["-"]
					&& sfxvY <= mouseY && mouseY <= sfxvY + textHeight) {
					pointer = true, offset = 90;
				}else offset = 0;

				TTF_DrawRendererText(setting_text("-", 200 - offset, 0, 0), x + textWidth["SFX Volume"], sfxvY);

				TTF_DrawRendererText(setting_text(sfxvStr, 255, 255, 255),
					x + textWidth["SFX Volume"] + 64 + textWidth["-"], sfxvY);

				if(x + textWidth["SFX Volume"] + 128 + textWidth[sfxvStr] + textWidth["-"] <= mouseX &&
					mouseX <= x + textWidth["SFX Volume"] + 128 + textWidth[sfxvStr] + textWidth["-"] + textWidth["+"]
					&& sfxvY <= mouseY && mouseY <= sfxvY + textHeight) {
					pointer = true, offset = 90;
				}else offset = 0;

				TTF_DrawRendererText(setting_text("+", 0, 200 - offset, 0), x + textWidth["SFX Volume"] + 128 + textWidth[sfxvStr] + textWidth["-"], sfxvY);


			}while(false);
		}


		if(pointer) {
			SDL_SetCursor(Cursor::pointerCursor);
		}else {
			SDL_SetCursor(Cursor::defaultCursor);
		}
	}

	void Main::mousedown(Uint8 button) {
		if(button != SDL_BUTTON_LEFT)
			return;
		float mouseX, mouseY;
		SDL_GetMouseState(&mouseX, &mouseY);
		if(!settingsUi && tx <= mouseX && mouseX <= tx + tw &&
			ty <= mouseY && mouseY <= ty + th) {
			SDL_SetCursor(Cursor::defaultCursor);
			funcSwitchUi(Game().id());
		}

		if(!settingsUi &&
			settingBtnRect.x <= mouseX && mouseX <= settingBtnRect.x + settingBtnRect.w &&
			settingBtnRect.y <= mouseY && mouseY <= settingBtnRect.y + settingBtnRect.h) {
			settingsUi = true;
			openTime = SDL_GetTicks();
		}

		if(settingsUi) {
			int elapsed = SDL_GetTicks() - openTime;
			float part = (float) std::min<int>(elapsed, fullOpenSettingsTime) / fullOpenSettingsTime;
			float mid = h / 2.0f;
			float workingH = h - 2.0f * settingMargin;

			SDL_FRect rect = {settingMargin, mid - part * workingH / 2.0f, w - 2.0f * settingMargin, part * workingH};

			float x = rect.x + 32;

			float backY = rect.y + 32;
			float resetY = backY + 32;
			float vsyncY = resetY + 128;

			const float dist = 36;
			float hpY = vsyncY + dist;
			float sgpY = hpY + dist;
			float npsY = sgpY + dist;
			float mvY = npsY + dist;
			float sfxvY = mvY + dist;
			float hsY = sfxvY + 2 * dist;

			if(x <= mouseX && mouseX <= x + textWidth["BACK"] &&
				backY <= mouseY && mouseY <= backY + textHeight && !closing) {
				closing = true;
				closeTime = SDL_GetTicks();
			}

			if(x <= mouseX && mouseX <= x + textWidth["RESET"] &&
				resetY <= mouseY && mouseY <= resetY + textHeight) {
				Settings::reset();
				Settings::update();
			}

			if(x <= mouseX && mouseX <= x + textWidth["CLEAR HIGH SCORES"] &&
				hsY <= mouseY && mouseY <= hsY + textHeight) {
				Settings::highScores.clear();
			}

			x += 32;
			static constexpr std::array<std::string, 2> boolText = {"Off", "On"};
			do {
				if(vsyncY + textHeight > rect.y + rect.h) break;

				if(x + textWidth["VSync"] <= mouseX && mouseX <= x + textWidth["VSync"] + textWidth[boolText[Settings::vsync]] &&
					vsyncY <= mouseY && mouseY <= vsyncY + textHeight) {
					Settings::vsync = !Settings::vsync;
					Settings::update();
				}

				if(hpY + textHeight > rect.y + rect.h) break;

				if(x + textWidth["Hold Ability"] <= mouseX && mouseX <= x + textWidth["Hold Ability"] + textWidth[boolText[Settings::holdAbility]] &&
					hpY <= mouseY && mouseY <= hpY + textHeight) {
					Settings::holdAbility = !Settings::holdAbility;
				}

				if(sgpY + textHeight > rect.y + rect.h) break;

				if(x + textWidth["Show Ghost Piece"] <= mouseX && mouseX <= x + textWidth["Show Ghost Piece"] + textWidth[boolText[Settings::ghostPiece]] &&
					sgpY <= mouseY && mouseY <= sgpY + textHeight) {
					Settings::ghostPiece = !Settings::ghostPiece;
				}

				if(npsY + textHeight > rect.y + rect.h) break;

				std::string npsStr = std::to_string(Settings::nextPieceCnt);

				if(x + textWidth["Next Piece Show Count"] <= mouseX &&
					mouseX <= x + textWidth["Next Piece Show Count"] + textWidth["-"]
					&& npsY <= mouseY && mouseY <= npsY + textHeight) {
					Settings::nextPieceCnt = std::max<int>(Settings::nextPieceCnt - 1, 0);
				}

				if(x + textWidth["Next Piece Show Count"] + 128 + textWidth[npsStr] + textWidth["-"] <= mouseX &&
					mouseX <= x + textWidth["Next Piece Show Count"] + 128 + textWidth[npsStr] + textWidth["-"] + textWidth["+"]
					&& npsY <= mouseY && mouseY <= npsY + textHeight) {
					Settings::nextPieceCnt = std::min<int>(Settings::nextPieceCnt + 1, 5);
				}


				if(mvY + textHeight > rect.y + rect.h) break;

				std::string mvStr = std::to_string(Settings::musicVolume) + "%";
				if(x + textWidth["Music Volume"] <= mouseX &&
					mouseX <= x + textWidth["Music Volume"] + textWidth["-"]
					&& mvY <= mouseY && mouseY <= mvY + textHeight) {
					Settings::musicVolume = std::max<int>(Settings::musicVolume - 5, 0);
					Settings::update();
				}

				if(x + textWidth["Music Volume"] + 128 + textWidth[mvStr] + textWidth["-"] <= mouseX &&
					mouseX <= x + textWidth["Music Volume"] + 128 + textWidth[mvStr] + textWidth["-"] + textWidth["+"]
					&& mvY <= mouseY && mouseY <= mvY + textHeight) {
					Settings::musicVolume = std::min<int>(Settings::musicVolume + 5, 100);
					Settings::update();
				}

				if(sfxvY + textHeight > rect.y + rect.h) break;

				std::string sfxvStr = std::to_string(Settings::sfxVolume) + "%";

				if(x + textWidth["SFX Volume"] <= mouseX &&
					mouseX <= x + textWidth["SFX Volume"] + textWidth["-"]
					&& sfxvY <= mouseY && mouseY <= sfxvY + textHeight) {
					Settings::sfxVolume = std::max<int>(Settings::sfxVolume - 5, 0);
					Settings::update();
				}

				if(x + textWidth["SFX Volume"] + 128 + textWidth[sfxvStr] + textWidth["-"] <= mouseX &&
					mouseX <= x + textWidth["SFX Volume"] + 128 + textWidth[sfxvStr] + textWidth["-"] + textWidth["+"]
					&& sfxvY <= mouseY && mouseY <= sfxvY + textHeight) {
					Settings::sfxVolume = std::min<int>(Settings::sfxVolume + 5, 100);
					Settings::update();
				}
			}while(false);
			Settings::save();
		}
	}

	void Main::destroy() {
		textWidth.clear();
		for(const auto& [str, ptr] : settingsText) {
			TTF_DestroyText(ptr);
		}
		settingsText.clear();

		TTF_DestroyText(playText);
		TTF_DestroyText(hsText);
		TTF_DestroyText(noneText);
	}
}