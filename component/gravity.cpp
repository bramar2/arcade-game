#include "gravity.hpp"

#include <cmath>

/*
  Implementation is adapted from https://{arcadegame}.wiki/Marathon, section Speed curve.
  Level is capped at 15. Assumes 64 fps.


	Level	Speed
	(unit: G)
	1	0.01667
	2	0.021017
	3	0.026977
	4	0.035256
	5	0.04693
	6	0.06361
	7	0.0879
	8	0.1236
	9	0.1775
	10	0.2598
	11	0.388
	12	0.59
	13	0.92
	14	1.46
	15	2.36
*/
namespace Gravity {
	int get_milliseconds(int level) {
		if(level < 1) level = 1;
		if(level > 15) level = 15;
		return std::ceil(1000.0 * std::pow(0.8 - ((level - 1) * 0.007), level - 1));
	}
}