#pragma once

#include <Core/World.hpp>

#include <optional>
#include <vector>

namespace sw::features {

	inline std::vector<uint32_t> filterValidTargets(
		uint32_t selfId,
		const std::vector<uint32_t>& candidateIds,
		const ::sw::core::WorldView& world)
	{
		std::vector<uint32_t> filteredTargets;
		for (uint32_t id : candidateIds) {
			// не атакуем себя
			if (id == selfId)
				continue;
			std::optional<int32_t> hp = world.getUnitHp(id);
			// не атакуем мертвых
			if (!hp.has_value() || *hp <= 0)
				continue;
			filteredTargets.push_back(id);
		}
		return filteredTargets;
	}
}
