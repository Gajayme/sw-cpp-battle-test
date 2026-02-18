#pragma once

#include <Features/Utils/TargetFilter.hpp>
#include <Core/Coord.hpp>
#include <Core/IBehavior.hpp>
#include <Core/Unit.hpp>
#include <IO/Events/UnitAttacked.hpp>
#include <IO/System/EventLog.hpp>

#include <cstdint>
#include <cstdlib>
#include <optional>

namespace sw::features {

	class MeleeAttackBehavior final : public ::sw::core::IBehavior {
	public:
		explicit MeleeAttackBehavior(int32_t damage) : _damage(damage) {}

		bool tryAct(::sw::core::Unit& self, ::sw::core::TurnContext& ctx) override {
			auto neighborIds = ctx.world.neighboringUnits(self.position());
			auto targetIds = filterValidTargets(self.id(), neighborIds, ctx.world);

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
		int32_t _damage{};
	};
}
