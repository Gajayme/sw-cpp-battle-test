#pragma once

namespace sw::core {

	class Unit;
	struct TurnContext;

	class IBehavior {
	public:
		virtual ~IBehavior() = default;

		// Возвращает true, если поведение выполнило действие в этом ходу, false в противном случае.
		virtual bool tryAct(Unit& self, TurnContext& ctx) = 0;
	};
}
