#pragma once

#include <Features/Utils/TargetFilter.hpp>
#include <Core/IBehavior.hpp>
#include <Core/Unit.hpp>
#include <IO/Events/UnitAttacked.hpp>
#include <IO/System/EventLog.hpp>

#include <cstdint>
#include <cstdlib>
#include <optional>

namespace sw::features {

	class RangedRingAttackBehavior final : public ::sw::core::IBehavior {
	public:
		RangedRingAttackBehavior(int32_t minDist, int32_t maxDist, int32_t damage, bool requireNoNeighbouringUnits)
			: _minDist(minDist)
			, _maxDist(maxDist)
			, _damage(damage)
			, _requireNoNeighbouringUnits(requireNoNeighbouringUnits)
		{}

		bool tryAct(::sw::core::Unit& self, ::sw::core::TurnContext& ctx) override {
			if (_requireNoNeighbouringUnits && ctx.world.hasNeighbouringBlockingUnit(self.position()))
				return false;

			auto inRangeIds = ctx.world.unitsInChebyshevRing(self.position(), _minDist, _maxDist);
			auto targetIds = filterValidTargets(self.id(), inRangeIds, ctx.world);

			// Если нет целей, то не атакуем
			if (targetIds.empty())
				return false;

			// Выбираем случайную цель
			uint32_t targetId = targetIds[static_cast<size_t>(std::rand()) % targetIds.size()];
			// Наносим урон
			ctx.world.changeHP(targetId, -_damage);
			// Логируем атаку
			std::optional<int32_t> newHp = ctx.world.getUnitHp(targetId);
			ctx.log.log(ctx.tick, ::sw::io::UnitAttacked{
					self.id(),
					targetId,
					static_cast<uint32_t>(_damage),
					static_cast<uint32_t>(newHp.value_or(0)),
				});
			return true;
		}

	private:
		int32_t _minDist{};
		int32_t _maxDist{};
		int32_t _damage{};
		bool _requireNoNeighbouringUnits{};
	};
}
