#pragma once

#include <string>
#include <functional>

namespace Scoring {
	unsigned long long calculate(std::function<void(std::string)> prompt, int grid[20][10], bool cleared[20], int level, int tSpin,
		int& combo, bool& btbDifficult, bool& btb4CPerfectClear);
	unsigned long long soft_drop(int cell);
	unsigned long long hard_drop(int cell);
}