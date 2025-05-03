#include "rotate.hpp"

#include <cassert>
#include <cmath>
#include <numbers>
#include <map>
#include <set>
#include <array>
#include <vector>
#include <utility>

/*
 Implementation uses the SRS standard (Super Rotation System),
 which is the current standard guideline for piece rotations.
 All information is from https://{arcadegame}.wiki/Super_Rotation_System

 T-spin checks are also in Rotate.cpp (https://{arcadegame}.wiki/T-Spin)
 T-spin = True if:
 - 3/4 squares diagonally adjacent to T's center are occupied
 - 2 front, 1-2 back => T-spin
 - 2 back, 1 front => Mini t-spin, unless the rotation is the 5th SRS rotation, in which case, it is considered a T-spin
*/
namespace Rotate {

	// map<{currentRotation, direction}, {the 5 test data}>
	static const std::map<
		std::array<int, 2>,
		std::array<std::array<int, 2>, 5>
		> STANDARD_WALL_KICK_DATA {
			{{0, +1}, { std::array<int, 2>{+0,+0},{-1,+0},{-1,+1},{+0,-2},{-1,-2} } },
			{{1, -1}, { std::array<int, 2>{+0,+0},{+1,+0},{+1,-1},{+0,+2},{+1,+2} } },
			{{1, +1}, { std::array<int, 2>{+0,+0},{+1,+0},{+1,-1},{+0,+2},{+1,+2} } },
			{{2, -1}, { std::array<int, 2>{+0,+0},{-1,+0},{-1,+1},{+0,-2},{-1,-2} } },
			{{2, +1}, { std::array<int, 2>{+0,+0},{+1,+0},{+1,+1},{+0,-2},{+1,-2} } },
			{{3, -1}, { std::array<int, 2>{+0,+0},{-1,+0},{-1,-1},{+0,+2},{-1,+2} } },
			{{3, +1}, { std::array<int, 2>{+0,+0},{-1,+0},{-1,-1},{+0,+2},{-1,+2} } },
			{{0, -1}, { std::array<int, 2>{+0,+0},{+1,+0},{+1,+1},{+0,-2},{+1,-2} } }
		};

	// for I tetromino
	static const std::map<
		std::array<int, 2>,
		std::array<std::array<int, 2>, 5>
		> I_WALL_KICK_DATA {
			{{0, +1}, { std::array<int, 2>{+0,+0},{-2,+0},{+1,+0},{-2,-1},{+1,+2} } },
			{{1, -1}, { std::array<int, 2>{+0,+0},{+2,+0},{-1,+0},{+2,+1},{-1,-2} } },
			{{1, +1}, { std::array<int, 2>{+0,+0},{-1,+0},{+2,+0},{-1,+2},{+2,-1} } },
			{{2, -1}, { std::array<int, 2>{+0,+0},{+1,+0},{-2,+0},{+1,-2},{-2,+1} } },
			{{2, +1}, { std::array<int, 2>{+0,+0},{+2,+0},{-1,+0},{+2,+1},{-1,-2} } },
			{{3, -1}, { std::array<int, 2>{+0,+0},{-2,+0},{+1,+0},{-2,-1},{+1,+2} } },
			{{3, +1}, { std::array<int, 2>{+0,+0},{+1,+0},{-2,+0},{+1,-2},{-2,+1} } },
			{{0, -1}, { std::array<int, 2>{+0,+0},{-1,+0},{+2,+0},{-1,+2},{+2,-1} } }
		};

	const std::vector<std::vector<int>> directions {{0, -1}, {0, 1}, {1, 0}, {-1, 0}};

	const int I_TETROMINO = 3; // turqoise
	const int T_TETROMINO = 4; // purple

	void mathRotate(int piece[4][2], int direction, double cx, double cy, int rotated[4][2]) {
		std::set<std::array<int, 2>> st;
		double angle = (direction == -1) ? 90.0 : -90.0;
		angle = angle * std::numbers::pi / 180.0;
		double cosTheta = std::cos(angle);
		double sinTheta = std::sin(angle);

		for(int p = 0; p < 4; p++) {
			double x = piece[p][0];
			double y = piece[p][1];

			x -= cx;
			y -= cy;

			// https://en.wikipedia.org/wiki/Rotation_(mathematics)
			int x2 = int(round(cx + x * cosTheta - y * sinTheta));
			int y2 = int(round(cy + x * sinTheta + y * cosTheta));


			rotated[p][0] = x2;
			rotated[p][1] = y2;

			st.insert({x2, y2});
		}

		assert(st.size() == 4);
	}

	bool getCenter(int piece[4][2], int currentRotation, double& cx, double& cy) {
		std::map<int, int> mp_x, mp_y;
		double sumX = 0, sumY = 0;
		for(int p = 0; p < 4; p++) {
			mp_x[piece[p][0]]++;
			mp_y[piece[p][1]]++;

			sumX += piece[p][0];
			sumY += piece[p][1];
		}

		if(mp_x.size() == 2 && mp_y.size() == 2) {
			return false; // O piece cannot rotate
		}



		cx = sumX / 4, cy = sumY / 4;

		if(mp_x.size() == 4) {
			cy += (currentRotation == 1) ? -0.5 : +0.5;
		}else if(mp_y.size() == 4) {
			cx += (currentRotation == 2) ? -0.5 : +0.5;
		}else if(mp_x.begin()->second == 3 || mp_x.rbegin()->second == 3) {

			if(mp_x.begin()->second != 1) cx = mp_x.begin()->first;
			else cx = mp_x.rbegin()->first;

		}else if(mp_y.begin()->second == 3 || mp_y.rbegin()->second == 3) {

			if(mp_y.begin()->second != 1) cy = mp_y.begin()->first;
			else cy = mp_y.rbegin()->first;

		}else if(currentRotation == 0) {
			cx = mp_x.rbegin()->first;
		}else if(currentRotation == 1) {
			cy = mp_y.begin()->first;
		}else if(currentRotation == 2) {
			cx = mp_x.begin()->first;
		}else if(currentRotation == 3) {
			cy = mp_y.rbegin()->first;
		}else {
			std::unreachable();
		}
		return true;
	}

	void checkTSpin(int grid[20][10], int piece[4][2], int currentRotation, bool lastRotation, int& tSpin) {
		tSpin = 0;
		double cx_d, cy_d;

		getCenter(piece, currentRotation, cx_d, cy_d);

		int cx = round(cx_d);
		int cy = round(cy_d);

		std::set<std::array<int, 2>> st;
		for(int p = 0; p < 4; p++) {
			st.insert({piece[p][0], piece[p][1]});
		}

		int backDirX = -10, backDirY = -10;
		for(const auto& dir : directions) {
			int x = cx + dir[0], y = cy + dir[1];
			if(!st.contains({x, y})) {
				backDirX = dir[0];
				backDirY = dir[1];
				break;
			}
		}
		assert(backDirX != -10);
		int front = 0, back = 0;

		if(backDirX == 0) {
			for(int offset = -1; offset <= 1; offset += 2) {
				int nx = cx + offset;
				int nyFront = cy - backDirY;
				int nyBack = cy + backDirY;

				if(!(0 <= nx && nx < 20 && 0 <= nyFront && nyFront < 10 && grid[nx][nyFront] == 0)) {
					front++;
				}
				if(!(0 <= nx && nx < 20 && 0 <= nyBack && nyBack < 10 && grid[nx][nyBack] == 0)) {
					back++;
				}
			}
		}else {
			for(int offset = -1; offset <= 1; offset += 2) {
				int ny = cy + offset;
				int nxFront = cx - backDirX;
				int nxBack = cx + backDirX;

				if(!(0 <= nxFront && nxFront < 20 && 0 <= ny && ny < 10 && grid[nxFront][ny] == 0)) {
					front++;
				}
				if(!(0 <= nxBack && nxBack < 20 && 0 <= ny && ny < 10 && grid[nxBack][ny] == 0)) {
					back++;
				}
			}
		}

		if(front == 2 && back >= 1) {
			tSpin = 1; // proper t-spin
		}else if(front == 1 && back >= 2) {
			if(lastRotation) {
				tSpin = 1; // proper t-spin
			}else {
				tSpin = 2; // mini t-spin
			}
		}
	}

	bool try_rotate(int grid[20][10], int piece[4][2], int currentPieceType, int currentRotation, int direction, int& tSpin) {
		assert(STANDARD_WALL_KICK_DATA.contains({currentRotation, direction}));
		assert(I_WALL_KICK_DATA.contains({currentRotation, direction}));

		double cx, cy;
		if(!getCenter(piece, currentRotation, cx, cy)) {
			return false;
		}
		std::set<std::array<int, 2>> st;
		for(int p = 0; p < 4; p++) {
			st.insert({piece[p][0], piece[p][1]});
		}

		int rotated[4][2];
		mathRotate(piece, direction, cx, cy, rotated);

		bool i_piece = (currentPieceType == I_TETROMINO);
		const auto& checks = (i_piece ? I_WALL_KICK_DATA : STANDARD_WALL_KICK_DATA).at({currentRotation, direction});
		for(int idx = 0; idx < 5; idx++) {
			// intentional dy, dx
			auto [dy, dx] = checks[idx];
			dx = -dx; // because row 0 to 20 is reversed
			
			int shifted[4][2];
			bool possible = true;
			for(int p = 0; p < 4; p++) {
				shifted[p][0] = rotated[p][0] + dx;
				shifted[p][1] = rotated[p][1] + dy;

				if(!(0 <= shifted[p][0] && shifted[p][0] < 20 &&
					0 <= shifted[p][1] && shifted[p][1] < 10)) {
					possible = false;
					break;
				}

				if(!st.contains({shifted[p][0], shifted[p][1]}) &&
					grid[shifted[p][0]][shifted[p][1]] != 0) {
					possible = false;
					break;
				}
			}
			if(possible) {
				for(int p = 0; p < 4; p++) {
					grid[piece[p][0]][piece[p][1]] = 0;
				}
				for(int p = 0; p < 4; p++) {
					grid[shifted[p][0]][shifted[p][1]] = currentPieceType;
					piece[p][0] = shifted[p][0];
					piece[p][1] = shifted[p][1];
				}
				if(currentPieceType == T_TETROMINO) {
					checkTSpin(grid, piece, (currentRotation + direction + 4) % 4, idx == 4, tSpin);
				}
				return true;
			}
		}


		return false;
	}
}