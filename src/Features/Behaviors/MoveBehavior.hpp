#pragma once

#include <Core/Coord.hpp>
#include <Core/GridMap.hpp>
#include <Core/IBehavior.hpp>
#include <Core/Unit.hpp>
#include <Core/World.hpp>
#include <Features/Utils/Pathfinding.hpp>
#include <IO/Events/MarchEnded.hpp>
#include <IO/Events/UnitMoved.hpp>
#include <IO/System/EventLog.hpp>

namespace sw::features {

	class MoveBehavior final : public ::sw::core::IBehavior {
	public:
		static constexpr int32_t DefaultStepsPerTurn = 1;

		explicit MoveBehavior(int32_t stepsPerTurn = DefaultStepsPerTurn)
			: _stepsPerTurn(stepsPerTurn > 0 ? stepsPerTurn : DefaultStepsPerTurn)
		{}

		bool tryAct(::sw::core::Unit& self, ::sw::core::TurnContext& ctx) override {
			auto target = self.marchTarget();

			if (!target)
				return false;

			const ::sw::core::GridMap& map = ctx.world.map();
			// Подвинулся ли юнит?
			bool moved = false;

			for (int32_t step = 0; step < _stepsPerTurn; ++step) {
				const ::sw::core::Coord from = self.position();
				if (from == *target) {
					ctx.world.clearMarch(self.id());
					break;
				}

				// Есть ли шаг?
				bool foundStep = false;
				for (const ::sw::core::Coord to : candidateStepsSorted(from, *target, map)) {
					if (self.blocksCell() && map.isOccupied(to))
						continue;

					ctx.world.applyMove(self, to);
					ctx.log.log(ctx.tick, ::sw::io::UnitMoved{
							self.id(),
							static_cast<uint32_t>(to.x),
							static_cast<uint32_t>(to.y),
						});
					moved = true;
					foundStep = true;

					if (to == *target) {
						ctx.log.log(ctx.tick, ::sw::io::MarchEnded{
								self.id(),
								static_cast<uint32_t>(to.x),
								static_cast<uint32_t>(to.y),
							});
						ctx.world.clearMarch(self.id());
					}
					break;
				}
				if (!foundStep)
					break;
				if (!self.marchTarget())
					break;
			}
			return moved;
		}

	private:
		int32_t _stepsPerTurn;
	};
}
