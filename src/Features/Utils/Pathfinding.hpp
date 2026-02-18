#pragma once

#include <Core/Coord.hpp>
#include <Core/GridMap.hpp>

#include <algorithm>
#include <vector>

namespace sw::features {

	inline std::vector<::sw::core::Coord> candidateStepsSorted(
		const ::sw::core::Coord& from,
		const ::sw::core::Coord& target,
		const ::sw::core::GridMap& map)
	{
		std::vector<::sw::core::Coord> candidates;

		// Проходим по всем возможным клеткам вокруг текущей
		for (int32_t dy = -1; dy <= 1; ++dy) {
			for (int32_t dx = -1; dx <= 1; ++dx) {
				if (dx == 0 && dy == 0)
					continue;

				const ::sw::core::Coord neighborCoord{from.x + dx, from.y + dy};

				if (!map.inBounds(neighborCoord))
					continue;
				candidates.push_back(neighborCoord);
			}
		}

		// Сортируем клетки по дистанции до цели
		std::sort(candidates.begin(), candidates.end(), [&](const ::sw::core::Coord& a, const ::sw::core::Coord& b) {
			const int32_t distanceToTargetA = ::sw::core::chebyshevDistance(a, target);
			const int32_t distanceToTargetB = ::sw::core::chebyshevDistance(b, target);
			if (distanceToTargetA != distanceToTargetB) return distanceToTargetA < distanceToTargetB;
			const int32_t distanceToTargetXA = std::abs(a.x - target.x);
			const int32_t distanceToTargetYA = std::abs(a.y - target.y);
			const int32_t distanceToTargetXB = std::abs(b.x - target.x);
			const int32_t distanceToTargetYB = std::abs(b.y - target.y);
			if (distanceToTargetXA != distanceToTargetXB) return distanceToTargetXA < distanceToTargetXB;
			return distanceToTargetYA < distanceToTargetYB;
		});
		return candidates;
	}
}
