#pragma once

#include <cstdint>
#include <cstdlib>

namespace sw::core {

	struct Coord {
		int32_t x{};
		int32_t y{};

		friend bool operator==(const Coord& a, const Coord& b) { return a.x == b.x && a.y == b.y; }
		friend bool operator!=(const Coord& a, const Coord& b) { return !(a == b); }
	};

	inline int32_t chebyshevDistance(const Coord& a, const Coord& b) {
		const int32_t dx = std::abs(a.x - b.x);
		const int32_t dy = std::abs(a.y - b.y);
		return dx > dy ? dx : dy;
	}
}

