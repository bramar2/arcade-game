#include "game.hpp"
#include "gravity.hpp"
#include "main.hpp"
#include "rotate.hpp"
#include "scoring.hpp"
#include "grid_test.hpp"

#include "../text/f1.hpp"
#include "../sound/theme.hpp"
#include "../sound/blockout.hpp"
#include "../sound/clear_four.hpp"
#include "../sound/hard_drop.hpp"
#include "../sound/line_clear.hpp"
#include "../sound/lock.hpp"
#include "../sound/move.hpp"
#include "../sound/rotate.hpp"
#include "../cursor/cursor.hpp"
#include "../blocks/red.hpp"
#include "../blocks/green.hpp"
#include "../blocks/blue.hpp"
#include "../blocks/orange.hpp"
#include "../blocks/purple.hpp"
#include "../blocks/yellow.hpp"
#include "../blocks/turqoise.hpp"
#include "../settings/settings.hpp"

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <algorithm>
#include <set>
#include <chrono>

namespace Ui {
	void Game::init(SDL_Window* i_window, SDL_Renderer* i_renderer, TTF_TextEngine* i_textEngine, const SwitchFunc& i_funcSwitchUi) {
		window = i_window;
		renderer = i_renderer;
		textEngine = i_textEngine;
		funcSwitchUi = i_funcSwitchUi;


		blocksArray = {
			nullptr,
			Block::Red::texture,
			Block::Blue::texture,
			Block::Turqoise::texture,
			Block::Purple::texture,
			Block::Orange::texture,
			Block::Yellow::texture,
			Block::Green::texture
		};

		ghostBlocksArray = {
			nullptr,
			Block::Red::ghostTexture,
			Block::Blue::ghostTexture,
			Block::Turqoise::ghostTexture,
			Block::Purple::ghostTexture,
			Block::Orange::ghostTexture,
			Block::Yellow::ghostTexture,
			Block::Green::ghostTexture
		};

		nextText = TTF_CreateText(textEngine, Font1::sz24, "NEXT", 0);
		TTF_SetTextColor(nextText, 255, 255, 255, 255);
		TTF_GetStringSize(Font1::sz24, "NEXT", 0, &nextTextSize[0], &nextTextSize[1]);

		holdText = TTF_CreateText(textEngine, Font1::sz24, "HOLD", 0);
		TTF_SetTextColor(holdText, 255, 255, 255, 255);
		TTF_GetStringSize(Font1::sz24, "HOLD", 0, &holdTextSize[0], &holdTextSize[1]);

		levelText = TTF_CreateText(textEngine, Font1::sz24, "LEVEL", 0);
		TTF_SetTextColor(levelText, 255, 255, 255, 255);
		TTF_GetStringSize(Font1::sz24, "LEVEL", 0, &levelTextSize[0], &levelTextSize[1]);

		linesText = TTF_CreateText(textEngine, Font1::sz24, "LINES", 0);
		TTF_SetTextColor(linesText, 255, 255, 255, 255);
		TTF_GetStringSize(Font1::sz24, "LINES", 0, &linesTextSize[0], &linesTextSize[1]);

		scoreText = TTF_CreateText(textEngine, Font1::sz24, "SCORE", 0);
		TTF_SetTextColor(scoreText, 255, 255, 255, 255);
		TTF_GetStringSize(Font1::sz24, "SCORE", 0, &scoreTextSize[0], &scoreTextSize[1]);

		loseText = TTF_CreateText(textEngine, Font1::sz24, "YOU LOST!", 0);
		TTF_SetTextColor(loseText, 255, 255, 255, 255);
		TTF_GetStringSize(Font1::sz24, "YOU LOST!", 0, &loseTextSize[0], &loseTextSize[1]);

		retryText = TTF_CreateText(textEngine, Font1::sz24, "RETRY", 0);
		TTF_SetTextColor(retryText, 255, 255, 255, 255);
		TTF_GetStringSize(Font1::sz24, "RETRY", 0, &retryTextSize[0], &retryTextSize[1]);
	}

	void Game::open() {
		std::fill(&grid[0][0], &grid[0][0] + sizeof(grid) / sizeof(grid[0][0]), 0);
		std::fill(&grid_ghost[0][0], &grid_ghost[0][0] + sizeof(grid_ghost) / sizeof(grid_ghost[0][0]), 0);
		currentPieceType = -1;
		tSpin = 0;
		retryUi = false;
		frameDurations = std::deque<Uint64>(frameDurationsSz, 0);
		frameDurationSum = 0;


		// testing
		// std::copy(&Test::mini_t_spin_single::grid[0][0], &Test::mini_t_spin_single::grid[0][0] + sizeof(grid) / sizeof(grid[0][0]), &grid[0][0]);
		// nextPieceTypes.push_front(Test::mini_t_spin_single::piece);


		dataLines = 0;
		dataScore = 0;
		combo = -1;
		btbDifficult = btb4CPerfectClear = false;

		Sound::start_theme();
		new_block();
	}

	void Game::render() {
		if(showFps) {
			if(lastRender == 0) {
				lastRender = static_cast<Uint64>(std::chrono::duration_cast<std::chrono::nanoseconds>(
					std::chrono::high_resolution_clock::now().time_since_epoch()).count());
			}else {
				Uint64 thisRender = static_cast<Uint64>(std::chrono::duration_cast<std::chrono::nanoseconds>(
					std::chrono::high_resolution_clock::now().time_since_epoch()).count());
				Uint64 duration = thisRender - lastRender;

				frameDurationSum = frameDurationSum + duration - frameDurations.front();
				frameDurations.pop_front();
				frameDurations.push_back(duration);

				fps = (1.0 * frameDurationsSz) / (frameDurationSum / 1'000'000'000.0);
				lastRender = thisRender;
			}
		}
		


		Uint64 tick = SDL_GetTicks();
		Sound::tick_theme();

		if(rowDropStage && tick >= lastRowDrop + dropTime) {
			row_drop_tick();
		}

		if(whooshStage != -1 && tick >= lastWhoosh + whooshTime) {
			whoosh_tick();	
		}
		
		key_tick();

		if(currentPieceType != -1 && tick - lastDropTick >= Gravity::get_milliseconds(dataLevel))
			drop(true);


		SDL_FRect mainBox {
			side, vertMargin, w - 2 * side, h - 2 * vertMargin
		};
		const float each = std::min(
			mainBox.w / 10.0,
			mainBox.h / 20.0
		);

		SDL_SetRenderDrawColor(renderer, 35, 35, 35, 255);
		for(int r = 0; r < 20; r++) {
			for(int c = 0; c < 10; c++) {
				SDL_FRect box { 
					side + c * each, vertMargin + r * each, each, each
				};
				SDL_RenderRect(renderer, &box);

				SDL_FRect objBox { 
					side + c * each + 1, vertMargin + r * each + 1, each - 2, each - 2
				};
				if(blocksArray[grid[r][c]] != nullptr) {
					SDL_RenderTexture(renderer, blocksArray[grid[r][c]], NULL, &objBox);
				}
				if(Settings::ghostPiece && ghostBlocksArray[grid_ghost[r][c]] != nullptr) {
					SDL_RenderTexture(renderer, ghostBlocksArray[grid_ghost[r][c]], NULL, &objBox);
				}
			}
		}

		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

		mainBox = {
			side - rEps, vertMargin - rEps, 10*each + 2 * rEps, 20*each + 2 * rEps
		};
		SDL_RenderRect(renderer, &mainBox);

		if(0 < Settings::nextPieceCnt && Settings::nextPieceCnt < 10) {
			float l = mainBox.x + mainBox.w, r = w;
			TTF_DrawRendererText(nextText, (l + r - nextTextSize[0]) / 2, vertMargin + 50);

			SDL_FRect nextBox {
				mainBox.x + mainBox.w + sideBoxGap,
				vertMargin + 50 + nextTextSize[1] + sideBoxVertGap,
				(w - (mainBox.x + mainBox.w + sideBoxGap)) - sideBoxGap,
				0
			};

			const float eachNext = nextBox.w / 6;

			nextBox.h = (3*Settings::nextPieceCnt + 1) * eachNext;

			SDL_RenderRect(renderer, &nextBox);

			for(int p = 0, sz = std::min<int>(Settings::nextPieceCnt, nextPieceTypes.size()); p < sz; p++) {
				int type = nextPieceTypes[p];
				for(int i = 0; i < 4; i++) {
					float r = blockStarts[type][i][0];
					float c = blockStarts[type][i][1];

					c -= 3;	// (0,3) at top left

					r += 1, c += 1;
					r += 3 * p;

					SDL_FRect objBox {
						nextBox.x + c * eachNext + 1, nextBox.y + r * eachNext + 1, eachNext - 2, eachNext - 2
					};
					SDL_RenderTexture(renderer, blocksArray[type], NULL, &objBox);
				}
			}
		}
		// hold
		float yBottomHold = mainBox.y + 16;
		if(Settings::holdAbility) {
			float l = 0, r = mainBox.x;
			TTF_DrawRendererText(holdText, (l + r - holdTextSize[0]) / 2, vertMargin + 50);

			SDL_FRect holdBox {
				sideBoxGap,
				vertMargin + 50 + holdTextSize[1] + sideBoxVertGap,
				mainBox.x - 2 * sideBoxGap,
				0
			};

			const float eachHold = holdBox.w / 6;

			holdBox.h = 4 * eachHold;

			SDL_RenderRect(renderer, &holdBox);

			if(heldPiece != -1) {
				int type = heldPiece;
				for(int i = 0; i < 4; i++) {
					float r = blockStarts[type][i][0];
					float c = blockStarts[type][i][1];

					c -= 3;	// (0,3) at top left
					r += 1, c += 1;

					SDL_FRect objBox {
						holdBox.x + c * eachHold + 1, holdBox.y + r * eachHold + 1, eachHold - 2, eachHold - 2
					};
					SDL_RenderTexture(renderer, blocksArray[type], NULL, &objBox);
				}
			}
			yBottomHold = holdBox.y + holdBox.h;
		}
		// data
		// level, lines, score
		std::string levelStr = std::to_string(dataLevel);
		std::string linesStr = std::to_string(dataLines);
		std::string scoreStr = std::to_string(dataScore);

		const int minDataLength = 8;
		if(linesStr.size() < minDataLength) {
			linesStr = std::string(minDataLength - linesStr.size(), '0') + linesStr;
		}
		if(scoreStr.size() < minDataLength) {
			scoreStr = std::string(minDataLength - scoreStr.size(), '0') + scoreStr;
		}

		int levelDataSize[2], linesDataSize[2], scoreDataSize[2];
		TTF_GetStringSize(Font1::sz18, levelStr.data(), 0, &levelDataSize[0], &levelDataSize[1]);
		TTF_GetStringSize(Font1::sz18, linesStr.data(), 0, &linesDataSize[0], &linesDataSize[1]);
		TTF_GetStringSize(Font1::sz18, scoreStr.data(), 0, &scoreDataSize[0], &scoreDataSize[1]);

		TTF_Text* levelDataText = TTF_CreateText(textEngine, Font1::sz18, levelStr.data(), 0);
		TTF_Text* linesDataText = TTF_CreateText(textEngine, Font1::sz18, linesStr.data(), 0);
		TTF_Text* scoreDataText = TTF_CreateText(textEngine, Font1::sz18, scoreStr.data(), 0);


		float levelTitleX = (side - levelTextSize[0]) / 2.0, levelTitleY = yBottomHold + 16;
		float levelDataX = std::max<float>(0, (side - levelDataSize[0]) / 2.0), levelDataY = levelTitleY + levelTextSize[1] + sideInfoDataGap;

		float linesTitleX = (side - linesTextSize[0]) / 2.0, linesTitleY = levelDataY + levelDataSize[1] + sideInfoInfoGap;
		float linesDataX = std::max<float>(0, (side - linesDataSize[0]) / 2.0), linesDataY = linesTitleY + linesTextSize[1] + sideInfoDataGap;

		float scoreTitleX = (side - scoreTextSize[0]) / 2.0, scoreTitleY = linesDataY + linesDataSize[1] + sideInfoInfoGap;
		float scoreDataX = std::max<float>(0, (side - scoreDataSize[0]) / 2.0), scoreDataY = scoreTitleY + scoreTextSize[1] + sideInfoDataGap;

		float bottom = scoreDataY + scoreDataSize[1];
		float targetBottom = mainBox.y + mainBox.h - sideGapFromBottom;

		if(bottom < targetBottom) {
			float add = targetBottom - bottom;
			levelTitleY += add, levelDataY += add;
			linesTitleY += add, linesDataY += add;
			scoreTitleY += add, scoreDataY += add;
		}


		TTF_DrawRendererText(levelText, levelTitleX, levelTitleY);
		TTF_DrawRendererText(levelDataText, levelDataX, levelDataY);

		TTF_DrawRendererText(linesText, linesTitleX, linesTitleY);
		TTF_DrawRendererText(linesDataText, linesDataX, linesDataY);

		TTF_DrawRendererText(scoreText, scoreTitleX, scoreTitleY);
		TTF_DrawRendererText(scoreDataText, scoreDataX, scoreDataY);

		TTF_DestroyText(levelDataText);
		TTF_DestroyText(linesDataText);
		TTF_DestroyText(scoreDataText);


		if(showFps) {
			if(tick - lastFpsUpd >= 100) {
				displayFps = fps;
				lastFpsUpd = tick;
			}
			std::string fpsStr = std::to_string((long long)(displayFps)) + " FPS";
			TTF_Text* fpsDataText = TTF_CreateText(textEngine, Font1::sz18, fpsStr.data(), 0);
			TTF_DrawRendererText(fpsDataText, 0, 0);
			TTF_DestroyText(fpsDataText);
		}


		if(retryUi) {
			// gray overlay
			SDL_BlendMode blendMode;
			SDL_GetRenderDrawBlendMode(renderer, &blendMode);
			SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
			SDL_SetRenderDrawColor(renderer, 0, 0, 0, overlayAlpha);
			SDL_FRect rect {0, 0, (float) w, (float) h};
			SDL_RenderFillRect(renderer, &rect);
			SDL_SetRenderDrawBlendMode(renderer, blendMode);

			std::string scoreText = "SCORE: " + std::to_string(dataScore);
			TTF_Text* scoreTtfText = TTF_CreateText(textEngine, Font1::sz18, scoreText.data(), 0);
			int scoreW, scoreH;
			TTF_GetStringSize(Font1::sz18, scoreText.data(), 0, &scoreW, &scoreH);

			float loseX = (w - loseTextSize[0]) / 2.0, loseY = h / 3.0;
			float scoreX = (w - scoreW) / 2.0, scoreY = loseY + loseTextSize[1] + 25.0 * h / 840;
			float retryX = (w - retryTextSize[0]) / 2.0, retryY = scoreY + scoreH + 80.0 * h / 840;

			float mouseX, mouseY;
			SDL_GetMouseState(&mouseX, &mouseY);

			int r = 255, g = 255, b = 255;

			if(retryX <= mouseX && mouseX <= retryX + retryTextSize[0] &&
				retryY <= mouseY && mouseY <= retryY + retryTextSize[1]) {
				r -= 90, g -= 90, b -= 90;
				SDL_SetCursor(Cursor::pointerCursor);
			}else {
				SDL_SetCursor(Cursor::defaultCursor);
			}

			TTF_SetTextColor(retryText, r, g, b, 255);

			TTF_DrawRendererText(loseText, loseX, loseY);
			TTF_DrawRendererText(scoreTtfText, scoreX, scoreY);
			TTF_DrawRendererText(retryText, retryX, retryY);

			TTF_DestroyText(scoreTtfText);
		}
	}

	void Game::mousedown(Uint8 button) {
		if(button != SDL_BUTTON_LEFT || !retryUi)
			return;
		float mouseX, mouseY;
		SDL_GetMouseState(&mouseX, &mouseY);

		std::string scoreText = "SCORE: " + std::to_string(dataScore);
		int scoreW, scoreH;
		TTF_GetStringSize(Font1::sz18, scoreText.data(), 0, &scoreW, &scoreH);

		float loseX = (w - loseTextSize[0]) / 2.0, loseY = h / 3.0;
		float scoreX = (w - scoreW) / 2.0, scoreY = loseY + loseTextSize[1] + 25.0 * h / 840;
		float retryX = (w - retryTextSize[0]) / 2.0, retryY = scoreY + scoreH + 80.0 * h / 840;

		if(retryX <= mouseX && mouseX <= retryX + retryTextSize[0] &&
			retryY <= mouseY && mouseY <= retryY + retryTextSize[1]) {

			// go to main ui
			SDL_SetCursor(Cursor::defaultCursor);
			funcSwitchUi(Main().id());
		}
	}

	void Game::destroy() {
		TTF_DestroyText(nextText);
		TTF_DestroyText(holdText);
		TTF_DestroyText(levelText);
		TTF_DestroyText(scoreText);
		TTF_DestroyText(linesText);
	}

	void Game::key_tick() {
		if(currentPieceType == -1) return;

		Uint64 tick = SDL_GetTicks();
		if(down_pressed && tick - down_pressed >= 80 && tick - down_last >= 35) {
			down_last = tick;
			drop(false);
		}

		if(left_pressed && tick - left_first >= 250 && tick - left_last >= 35) {
			left_last = tick;
			try_move(0, -1);
		}

		if(right_pressed && tick - right_first >= 250 && tick - right_last >= 35) {
			right_last = tick;
			try_move(0, 1);
		}

		if(space_pressed && tick - space_first >= 250 && tick - space_last >= 120) {
			space_last = tick;
			hard_drop();
		}
	}

	void Game::new_block() {

		while((int) nextPieceTypes.size() < 10) {
			std::vector<int> perm {1, 2, 3, 4, 5, 6, 7};
			std::shuffle(perm.begin(), perm.end(), mt);
			for(int t : perm) nextPieceTypes.push_back(t);
		}

		int type = nextPieceTypes.front();
		nextPieceTypes.pop_front();

		bool good = true;
		for(int b = 0; b < 4; b++) {
			int r = blockStarts[type][b][0];
			int c = blockStarts[type][b][1];
			if(grid[r][c] == 0) {
				grid[r][c] = type;
				currentPiece[b][0] = r;
				currentPiece[b][1] = c;
			}else {
				good = false;
			}
		}

		if(good) {
			currentPieceType = type;
			currentPieceRotation = 0;
			holdAvailable = true;
			thisWasHardDrop = false;
			lastDropTick = SDL_GetTicks();
			tSpin = 0;
			calc_ghost();
		}else {
			lose();
		}

	}

	void Game::keyup(SDL_Keycode key) {
		switch(key) {
		case SDLK_LEFT:
			left_first = left_last = 0, left_pressed = false;
			break;
		case SDLK_RIGHT:
			right_first = right_last = 0, right_pressed = false;
			break;
		case SDLK_DOWN:
			down_first = down_last = 0, down_pressed = false;
			break;
		case SDLK_UP:
			up_pressed = false;
			break;
		case SDLK_LCTRL:
		case SDLK_Z:
			z_pressed = false;
			break;
		case SDLK_SPACE:
			space_first = space_last = 0, space_pressed = false;
			break;
		default:
			break;
		}
	}

	void Game::keydown(SDL_Keycode key) {
		switch(key) {
		case SDLK_LEFT:
			if(!left_pressed) {
				try_move(0, -1);
				left_first = left_last = SDL_GetTicks();
				left_pressed = true;
			}
			break;
		case SDLK_RIGHT:
			if(!right_pressed) {
				try_move(0, 1);
				right_first = right_last = SDL_GetTicks();
				right_pressed = true;
			}
			break;
		case SDLK_DOWN:
			if(!down_pressed) {
				drop(false);
				down_first = down_last = SDL_GetTicks();
				down_pressed = true;
			}
			break;
		case SDLK_UP:
			if(!up_pressed) {
				rotate(-1);
				up_pressed = true;
			}
			break;
		case SDLK_LCTRL:
		case SDLK_Z:
			if(!z_pressed) {
				rotate(1);
				z_pressed = true;
			}
			break;
		case SDLK_C:
		case SDLK_LSHIFT:
			if(holdAvailable) {
				hold();
				holdAvailable = false;
			}
			break;
		case SDLK_SPACE:
			if(!space_pressed) {
				hard_drop();
				space_first = space_last = SDL_GetTicks();
				space_pressed = true;
			}
			break;
		default:
			break;
		}
	}

	void Game::drop(bool force) {
		if(try_move(1, 0)) {
			if(!force) {
				dataScore += Scoring::soft_drop(1);
			}
			lastDropTick = SDL_GetTicks();
		}else if(force) {
			// block is placed
			Sound::play_lock();
			whoosh();
		}
	}

	void Game::hard_drop() {
		if(currentPieceType == -1) return;


		int distRow = std::abs(currentPiece[0][0] - currentGhost[0][0]);
		dataScore += Scoring::hard_drop(distRow);

		for(int p = 0; p < 4; p++) {
			grid[currentPiece[p][0]][currentPiece[p][1]] = 0;
		}

		for(int p = 0; p < 4; p++) {
			currentPiece[p][0] = currentGhost[p][0];
			currentPiece[p][1] = currentGhost[p][1];
		}

		for(int p = 0; p < 4; p++) {
			grid[currentPiece[p][0]][currentPiece[p][1]] = currentPieceType;
		}

		lastDropTick = 1;
		Sound::play_hard_drop();
		thisWasHardDrop = true;
		drop(true);
	}

	void Game::rotate(int direction) { // 1 = clockwise, -1 = counter-clockwise
		if(currentPieceType == -1) return;
		if(Rotate::try_rotate(grid, currentPiece, currentPieceType, currentPieceRotation, direction, tSpin)) {
			currentPieceRotation = (currentPieceRotation + direction + 4) % 4;
			Sound::play_rotate();
			calc_ghost();
		}
	}

	bool Game::try_move(int dr, int dc) {
		if(currentPieceType == -1) return false;

		std::set<std::array<int, 2>> st;
		for(int b = 0; b < 4; b++) {
			st.insert({currentPiece[b][0], currentPiece[b][1]});
		}
		for(int b = 0; b < 4; b++) {
			if(st.count({currentPiece[b][0] + dr, currentPiece[b][1] + dc}))
				continue;
			if(grid[currentPiece[b][0] + dr][currentPiece[b][1] + dc] != 0) {
				return false;
			}

			if(!(
				0 <= currentPiece[b][0] + dr && currentPiece[b][0] + dr < 20 &&
				0 <= currentPiece[b][1] + dc && currentPiece[b][1] + dc < 10
				))
				return false;
		}

		for(int b = 0; b < 4; b++) {
			grid[currentPiece[b][0]][currentPiece[b][1]] = 0;
		}
		for(int b = 0; b < 4; b++) {
			currentPiece[b][0] += dr;
			currentPiece[b][1] += dc;
			grid[currentPiece[b][0]][currentPiece[b][1]] = currentPieceType;
		}

		tSpin = 0;
		if(dr == 0) Sound::play_move();
		calc_ghost();
		return true;
	}

	void Game::calc_ghost() {
		for(int p = 0; p < 4; p++) {
			grid_ghost[currentGhost[p][0]][currentGhost[p][1]] = 0;
			currentGhost[p][0] = currentPiece[p][0];
			currentGhost[p][1] = currentPiece[p][1];
		}
		while(true) {
			std::set<std::array<int, 2>> st;
			for(int p = 0; p < 4; p++) {
				st.insert({currentGhost[p][0], currentGhost[p][1]});
			}
			bool possible = true;
			for(int p = 0; p < 4; p++) {
				if(st.count({currentGhost[p][0] + 1, currentGhost[p][1]}))
					continue;
				if(0 <= currentGhost[p][0] + 1 && currentGhost[p][0] + 1 < 20 &&
					0 <= currentGhost[p][1] && currentGhost[p][1] < 10 &&
					grid[currentGhost[p][0] + 1][currentGhost[p][1]] == 0)
					continue;
				possible = false;
			}
			if(possible) {
				for(int p = 0; p < 4; p++)
					currentGhost[p][0]++;
			}else {
				break;
			}
		}
		for(int p = 0; p < 4; p++) {
			grid_ghost[currentGhost[p][0]][currentGhost[p][1]] = currentPieceType;
		}
	}

	void prompt(std::string str) {
		SDL_Log("Score: %s", str.data());
	}

	void Game::whoosh() {
		std::fill(std::begin(full), std::end(full), true);
		for(int r = 0; r < 20; r++) {
			for(int c = 0; c < 10; c++) {
				if(grid[r][c] == 0) {
					full[r] = false;
					break;
				}
			}
		}

		int clearedCount = std::count(std::begin(full), std::end(full), true);
		
		dataScore += Scoring::calculate(prompt, grid, full, dataLevel, tSpin, combo, btbDifficult, btb4CPerfectClear);
		dataLines += clearedCount;

		if(dataLevel != 1 + (dataLines / levelLines)) {
			// level up
			dataLevel = 1 + (dataLines / 10);
		}

		if(clearedCount == 4) {
			Sound::play_clear_four();
		}
		if(clearedCount > 0) {
			Sound::play_line_clear();
			whooshedAny = true;
		}else {
			whooshedAny = false;
			if(thisWasHardDrop) { // insta-spawn
				new_block();
				return;
			}
		}

		whooshStage = 0;
		lastWhoosh = 0;
		currentPieceType = -1;
	}

	void Game::whoosh_tick() {
		if(whooshStage == 10) {
			whooshStage = -1;
			row_drop();
			return;
		}

		for(int r = 0; r < 20; r++) {
			if(!full[r]) continue;
			grid[r][whooshStage] = 0;
		}

		whooshStage++;
		lastWhoosh = SDL_GetTicks();
	}

	void Game::row_drop() {
		rowDropStage = true;
		lastRowDrop = 0;
	}

	void Game::row_drop_tick() {
		bool moved = false;
		for(int r = 19; r >= 1; r--) {
			bool isThisEmpty = true, isTopEmpty = true;
			for(int c = 0; c < 10; c++) {
				if(grid[r][c] != 0) {
					isThisEmpty = false;
				}
				if(grid[r - 1][c] != 0) {
					isTopEmpty = false;
				}
			}
			if(isThisEmpty && !isTopEmpty) { // drop 1
				moved = true;
				for(int c = 0; c < 10; c++) {
					grid[r][c] = grid[r - 1][c];
					grid[r - 1][c] = 0;
				}
			}
		}

		rowDropStage = moved;
		lastRowDrop = SDL_GetTicks();

		if(!rowDropStage) {
			new_block();
		}
	}

	void Game::lose() {
		Sound::stop_theme();
		Sound::play_blockout();
		currentPieceType = -1;
		retryUi = true;

		Settings::highScores.push_back(dataScore);
		std::sort(Settings::highScores.rbegin(), Settings::highScores.rend());
		while((int) Settings::highScores.size() > 5) Settings::highScores.pop_back();
		Settings::save();
	}

	void Game::hold() {
		if(currentPieceType == -1 || !Settings::holdAbility) return;

		if(heldPiece != -1) {
			nextPieceTypes.push_front(heldPiece);
		}
		heldPiece = currentPieceType;

		for(int p = 0; p < 4; p++) {
			int r = currentPiece[p][0], c = currentPiece[p][1];
			grid[r][c] = 0;
		}

		new_block();
		holdAvailable = false;
	}
}