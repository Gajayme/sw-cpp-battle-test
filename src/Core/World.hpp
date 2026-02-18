#pragma once

#include "Coord.hpp"
#include "GridMap.hpp"
#include "Unit.hpp"

#include <cstdint>
#include <memory>
#include <optional>
#include <unordered_map>
#include <vector>

namespace sw::core {

	class World {
	public:
		explicit World(GridMap map);

		const std::vector<std::unique_ptr<Unit>>& unitsInCreationOrder() const;

		void spawn(std::unique_ptr<Unit> unit);

		std::optional<int32_t> getUnitHp(uint32_t unitId) const;
		std::optional<Coord> getUnitPosition(uint32_t unitId) const;
		bool getUnitBlocksCell(uint32_t unitId) const;

		std::vector<uint32_t> neighboringUnits(const Coord& center);
		std::vector<uint32_t> unitsInChebyshevRing(const Coord& center, int32_t minD, int32_t maxD);
		bool hasNeighbouringBlockingUnit(const Coord& c);

		const GridMap& map() const;

		void applyMove(Unit& unit, const Coord& to);
		void changeUnitHp(uint32_t unitId, int32_t delta);
		void setUnitMarchTarget(uint32_t unitId, const Coord& target);
		void clearUnitMarch(uint32_t unitId);
		std::vector<uint32_t> removeDeadUnits();
		size_t aliveUnitsCount() const;

	private:
		Unit* getUnit(uint32_t id);
		const Unit* getUnit(uint32_t id) const;
		void removeUnit(uint32_t id);

		GridMap _map;
		std::vector<std::unique_ptr<Unit>> _units;
		std::unordered_map<uint32_t, size_t> _byId;
	};

	// WorldView с ограниченным доступом к миру
	class WorldView {
	public:
		explicit WorldView(World& world);

		const GridMap& map() const;
		std::vector<uint32_t> neighboringUnits(const Coord& center);
		std::vector<uint32_t> unitsInChebyshevRing(const Coord& center, int32_t minD, int32_t maxD);
		bool hasNeighbouringBlockingUnit(const Coord& c);
		void applyMove(Unit& unit, const Coord& to);

		void changeHP(uint32_t unitId, int32_t delta);
		void setMarchTarget(uint32_t unitId, const Coord& target);
		void clearMarch(uint32_t unitId);

		std::optional<int32_t> getUnitHp(uint32_t unitId) const;
		std::optional<Coord> getUnitPosition(uint32_t unitId) const;
		bool getUnitBlocksCell(uint32_t unitId) const;

	private:
		World& _world;
	};
}
