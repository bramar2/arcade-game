#include "scoring.hpp"

#include "../name/name.hpp"

#include <cmath>

/*
  Implementation is using the standard from https://{arcadegame}.wiki/Scoring

  Single						100 × level
  Double						300 × level
  Triple						500 × level
  4 Clear						800 × level; difficult
  Mini T-Spin no line(s)		100 × level
  T-Spin no line(s)				400 × level
  Mini T-Spin Single			200 × level; difficult
  T-Spin Single					800 × level; difficult
  Mini T-Spin Double 			400 × level; difficult
  T-Spin Double					1200 × level; difficult
  T-Spin Triple					1600 × level; difficult
  Back-to-Back difficult line clears	Action score × 1.5 (excluding soft drop and hard drop)
  Combo							50 × combo count × level
  Soft drop						1 per cell
  Hard drop						2 per cell

  Action	Points
  Single-line perfect clear	800 × level
  Double-line perfect clear	1200 × level
  Triple-line perfect clear	1800 × level
  4-line perfect clear 2000 × level
  Back-to-back 4-line perfect clear	3200 × level

*/
namespace Scoring {

	unsigned long long calculate(std::function<void(std::string)> prompt, int grid[20][10], bool cleared[20], int level, int tSpin,
    	int& combo, bool& btbDifficult, bool& btb4CPerfectClear) {

		int cnt = 0, nonEmptyCnt = 0;
		for(int r = 0; r < 20; r++) {
			if(cleared[r]) cnt++;

			bool empty = true;
			for(int c = 0; c < 10; c++) {
				if(grid[r][c] != 0) {
					empty = false;
					break;
				}
			}
			if(!empty) nonEmptyCnt++;
		}

		if(cnt == 0) {
			combo = -1;

			if(tSpin == 0) {
				return 0;
			}else if(tSpin == 1) { // proper
				prompt("T-Spin +" + std::to_string(400 * level));
				return 400 * level;
			}else { // mini
				prompt("Mini T-Spin +" + std::to_string(100 * level));
				return 100 * level;
			}
		}
		combo++;
		unsigned long long ans = 0;

		if(combo > 0) {
			prompt("Combo " + std::to_string(combo) + " +" + std::to_string(50 * combo * level));
			ans += 50 * combo * level;
		}

		bool difficult = false;
		if(cnt == 1) {

			if(tSpin == 0)
				ans += 100 * level, prompt("Single +" + std::to_string(100 * level));
			else if(tSpin == 1)
				ans += 800 * level, difficult = true, prompt("TS-Single +" + std::to_string(800 * level));
			else
				ans += 200 * level, difficult = true, prompt("MTS-Single +" + std::to_string(200 * level));

		}else if(cnt == 2) {

			if(tSpin == 0)
				ans += 300 * level, prompt("Double +" + std::to_string(300 * level));
			else if(tSpin == 1)
				ans += 1200 * level, difficult = true, prompt("TS-Double +" + std::to_string(1200 * level));
			else
				ans += 400 * level, difficult = true, prompt("MTS-Double +" + std::to_string(400 * level));

		}else if(cnt == 3) {

			if(tSpin == 0)
				ans += 500 * level, prompt("Triple +" + std::to_string(500 * level));
			else
				ans += 1600 * level, difficult = true, prompt("TS-Triple +" + std::to_string(1600 * level));

		}else {
			ans += 800 * level, difficult = true, prompt(Name::NAME + " +" + std::to_string(800 * level));
		}

		if(difficult) {
			if(btbDifficult) {
				prompt("Back-to-back x1.5");
				ans = ans * 3 / 2;
			}
			btbDifficult = true;
		}else {
			btbDifficult = false;
		}

		bool perfect = (cnt == nonEmptyCnt);

		if(perfect) {
			if(cnt == 1) {
				ans += 800 * level, btb4CPerfectClear = false;
				prompt("Perfect +" + std::to_string(800 * level));
			}else if(cnt == 2) {
				ans += 1200 * level, btb4CPerfectClear = false;
				prompt("Perfect +" + std::to_string(1200 * level));
			}else if(cnt == 3) {
				ans += 1800 * level, btb4CPerfectClear = false;
				prompt("Perfect +" + std::to_string(1800 * level));
			}else {
				if(btb4CPerfectClear) {
					ans += 3200 * level;
					prompt(Name::NAME + " Perfect B2B +" + std::to_string(3200 * level));
				}else {
					ans += 2000 * level;
					btb4CPerfectClear = false;
					prompt(Name::NAME + " Perfect +" + std::to_string(2000 * level));
				}
			}
		}else {
			btb4CPerfectClear = false;
		}

		return ans;
	}

	unsigned long long soft_drop(int cell) {
		return 1 * std::max<int>(cell, 0);
	}
	unsigned long long hard_drop(int cell) {
		return 2 * std::max<int>(cell, 0);
	}
}