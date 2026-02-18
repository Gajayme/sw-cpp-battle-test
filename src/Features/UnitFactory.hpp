#pragma once

#include <Core/Coord.hpp>
#include <Core/Unit.hpp>
#include <Features/Behaviors/MeleeAttackBehavior.hpp>
#include <Features/Behaviors/MoveBehavior.hpp>
#include <Features/Behaviors/RangedRingAttackBehavior.hpp>

#include <memory>

namespace sw::features {

	inline std::unique_ptr<::sw::core::Unit> createSwordsman(
		uint32_t id,
		::sw::core::Coord pos,
		int32_t hp,
		int32_t strength)
	{
		auto unit = std::make_unique<::sw::core::Unit>(id, "Swordsman", pos, hp, true);
		unit->addBehavior(std::make_unique<MeleeAttackBehavior>(strength));
		unit->addBehavior(std::make_unique<MoveBehavior>(1));
		return unit;
	}

	inline std::unique_ptr<::sw::core::Unit> createHunter(
		uint32_t id,
		::sw::core::Coord pos,
		int32_t hp,
		int32_t agility,
		int32_t strength,
		int32_t range)
	{
		auto unit = std::make_unique<::sw::core::Unit>(id, "Hunter", pos, hp, true);
		unit->addBehavior(std::make_unique<RangedRingAttackBehavior>(2, range, agility, true));
		unit->addBehavior(std::make_unique<MeleeAttackBehavior>(strength));
		unit->addBehavior(std::make_unique<MoveBehavior>(1));
		return unit;
	}
}
