#pragma once

#include "base.hpp"

#include <SDL3/SDL_audio.h>

#include <deque>
#include <random>
#include <array>

namespace Ui {
	class Game : public Base {
	private:
		std::random_device rd {};
		std::mt19937 mt { rd() };



		const int w = 840, h = 840;
		const float side = 240;
		const float vertMargin = 40;
		const float rEps = 3;
		const float sideBoxVertGap = 10;
		const float sideBoxGap = 25;
		const int overlayAlpha = 128;

		const float sideInfoDataGap = 15;
		const float sideInfoInfoGap = 50;
		const float sideGapFromBottom = 80;

		const int whooshTime = 20; // every `whooshTime`ms, a new column is cleared if its a full streak
		const int dropTime = 25; // after all cols wiped, there will possibly be floating blocks so it takes `dropTime`ms to drop 1 row.

		// Fixed-goal leveling system, every 10 lines, +1 level
		const int levelLines = 10;

		unsigned long long dataLevel = 1;
		unsigned long long dataLines = 0;
		unsigned long long dataScore = 0;
		int combo = -1;
		bool btbDifficult = false;
		bool btb4CPerfectClear = false;
		
		int grid[20][10] = {};
		int grid_ghost[20][10] = {};
		bool full[20] = {};

		bool thisWasHardDrop = false;
		int whooshStage = -1; // -1 is no, else next clear is `whooshStage` (0 => none cleared)
		bool whooshedAny = false;
		Uint64 lastWhoosh = 0;

		bool rowDropStage = false;
		Uint64 lastRowDrop 	= 0;

		SDL_Window* window;
		SDL_Renderer* renderer;
		TTF_TextEngine* textEngine;
		SwitchFunc funcSwitchUi;

		// fps

		const bool showFps = true;
		Uint64 lastFpsUpd = 0;
		double displayFps;
		Uint64 lastRender = 0;
		double fps = 0;
		const int frameDurationsSz = 30; // as a sample cnt for getting FPS
		std::deque<Uint64> frameDurations;
		Uint64 frameDurationSum = 0;

		//

		std::array<SDL_Texture*, 10> blocksArray, ghostBlocksArray;

		const int blockStarts[10][4][2] {
			{ {0, 0}, {0, 0}, {0, 0}, {0, 0}  }, // -
			{ {0, 5}, {0, 4}, {1, 5}, {1, 6} }, // red
			{ {0, 3}, {1, 3}, {1, 4}, {1, 5} }, // blue
			{ {0, 3}, {0, 4}, {0, 5}, {0, 6} }, // turqoise
			{ {0, 5}, {1, 5}, {1, 4}, {1, 6} }, // purple
			{ {0, 5}, {1, 5}, {1, 4}, {1, 3} }, // orange
			{ {0, 5}, {0, 4}, {1, 5}, {1, 4} }, // yellow
			{ {0, 5}, {1, 5}, {0, 6}, {1, 4} } // green
		};

		int currentPiece[4][2] {};
		int currentGhost[4][2] {};
		int currentPieceType = -1;
		// 0 = Default, 1 = Rotated once clockwise (R), 2 = 180deg, 3 = Rotated once counter-clockwise (L)
		int currentPieceRotation = -1;
		Uint64 lastDropTick = 0;
		int tSpin = 0; // 0 = NONE, 1 = PROPER, 2 = MINI

		std::deque<int> nextPieceTypes;

		// ticks of when l/r/d arrow keys are pressed
		// u = rotate, no hold system
		Uint64 left_first, left_last;
		Uint64 right_first, right_last;
		Uint64 down_first, down_last;
		Uint64 space_first, space_last;

		bool left_pressed = false;
		bool right_pressed = false;
		bool down_pressed = false;
		bool up_pressed = false;
		bool z_pressed = false;
		bool space_pressed = false;
		bool holdAvailable = true;

		TTF_Text* nextText;
		int nextTextSize[2];

		TTF_Text* holdText;
		int holdTextSize[2];

		TTF_Text* levelText;
		int levelTextSize[2];

		TTF_Text* scoreText;
		int scoreTextSize[2];

		TTF_Text* linesText;
		int linesTextSize[2];

		TTF_Text* loseText;
		int loseTextSize[2];

		TTF_Text* retryText;
		int retryTextSize[2];

		int heldPiece = -1;

		bool retryUi = false;
	public:
		int id() override { return 1; }

		void init(SDL_Window* window, SDL_Renderer* renderer, TTF_TextEngine* textEngine, const SwitchFunc& funcSwitchUi) override;
		void open() override;
		void render() override;
		void keyup(SDL_Keycode key) override;
		void keydown(SDL_Keycode key) override;
		void mousedown(Uint8 button) override;
		void destroy() override;

		// game mechanics
		void key_tick();
		void new_block();
		void drop(bool force);
		bool try_move(int dr, int dc);
		void rotate(int direction);
		void lose();
		void whoosh();
		void whoosh_tick();
		void row_drop();
		void hard_drop();
		void row_drop_tick();
		void hold();
		void calc_ghost();
	};
}