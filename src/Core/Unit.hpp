#pragma once

#include "Coord.hpp"
#include "IBehavior.hpp"

#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace sw {
	class EventLog;
}

namespace sw::core {

	class WorldView;

	struct TurnContext {
		WorldView& world;
		::sw::EventLog& log;
		uint64_t tick{};
	};

	class Unit {
		friend class World;

	public:
		Unit(uint32_t id, std::string typeName, Coord pos, int32_t hp, bool blocksCell)
			: _id(id)
			, _typeName(std::move(typeName))
			, _pos(pos)
			, _hp(hp)
			, _blocksCell(blocksCell)
		{}

		~Unit() = default;

		uint32_t id() const {
			return _id;
		}

		const std::string& typeName() const {
			return _typeName;
		}

		Coord position() const {
			return _pos;
		}

		int32_t hp() const {
			return _hp;
		}

		bool blocksCell() const {
			return _blocksCell;
		}

		std::optional<Coord> marchTarget() const {
			return _march;
		}

		void addBehavior(std::unique_ptr<IBehavior> behavior) {
			if (behavior) _behaviors.push_back(std::move(behavior));
		}

		bool takeTurn(TurnContext& ctx) {
			for (const auto& behavior : _behaviors) {
				if (!behavior)
					continue;
				if (behavior->tryAct(*this, ctx))
					return true;
			}
			return false;
		}

	private:
		void setHp(int32_t value) {
			_hp = value < 0 ? 0 : value;
		}
		void setMarchTarget(const Coord& target) {
			_march = target;
		}
		void clearMarch() {
			_march.reset();
		}
		void setPosition(const Coord& pos) {
			_pos = pos;
		}

	private:
		uint32_t _id{};
		std::string _typeName;
		Coord _pos{};
		int32_t _hp{0};
		bool _blocksCell{true};
		std::optional<sw::core::Coord> _march;
		std::vector<std::unique_ptr<IBehavior>> _behaviors;
	};
}

