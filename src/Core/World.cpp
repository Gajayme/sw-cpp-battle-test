#include "World.hpp"

#include <stdexcept>

namespace sw::core {

	World::World(GridMap map)
		: _map(std::move(map))
	{}

	const std::vector<std::unique_ptr<Unit>>& World::unitsInCreationOrder() const {
		return _units;
	}

	Unit* World::getUnit(uint32_t id) {
		auto it = _byId.find(id);
		if (it == _byId.end())
			return nullptr;
		return _units[it->second].get();
	}

	const Unit* World::getUnit(uint32_t id) const {
		auto it = _byId.find(id);
		if (it == _byId.end())
			return nullptr;
		return _units[it->second].get();
	}

	std::optional<int32_t> World::getUnitHp(uint32_t unitId) const {
		const Unit* u = getUnit(unitId);
		if (u)
			return u->hp();
		return std::nullopt;
	}

	std::optional<Coord> World::getUnitPosition(uint32_t unitId) const {
		const Unit* u = getUnit(unitId);
		if (u)
			return u->position();
		return std::nullopt;
	}

	bool World::getUnitBlocksCell(uint32_t unitId) const {
		const Unit* u = getUnit(unitId);
		return u && u->blocksCell();
	}

	void World::spawn(std::unique_ptr<Unit> unit) {
		if (!unit)
			throw std::runtime_error("spawn: unit is null");
		if (unit->hp() < 0)
			throw std::runtime_error("spawn: hp cannot be negative");
		if (_byId.contains(unit->id()))
			throw std::runtime_error("spawn: duplicate unit id");
		if (!_map.inBounds(unit->position()))
			throw std::runtime_error("spawn: out of bounds");
		if (unit->blocksCell() && _map.isOccupied(unit->position()))
			throw std::runtime_error("spawn: cell is occupied");

		const size_t idx = _units.size();
		_byId.emplace(unit->id(), idx);
		if (unit->blocksCell())
			_map.setOccupied(unit->position(), static_cast<int32_t>(unit->id()));
		_units.emplace_back(std::move(unit));
	}

	// Соседи по всем юнитам в 8 смежных клетках (и болкирующие клетку и нет)
	std::vector<uint32_t> World::neighboringUnits(const Coord& center) {
		return unitsInChebyshevRing(center, 1, 1);
	}

	std::vector<uint32_t> World::unitsInChebyshevRing(const Coord& center, int32_t minD, int32_t maxD) {
		std::vector<uint32_t> result;
		for (const auto& uptr : _units) {
			if (!uptr)
				continue;
			const Coord unitPosition = uptr->position();
			const int32_t distanceToUnit = chebyshevDistance(center, unitPosition);
			if (distanceToUnit >= minD && distanceToUnit <= maxD)
				result.push_back(uptr->id());
		}
		return result;
	}

	bool World::hasNeighbouringBlockingUnit(const Coord& coordinate) {
		for (uint32_t id : neighboringUnits(coordinate)) {
			if (getUnitBlocksCell(id))
				return true;
		}
		return false;
	}

	const GridMap& World::map() const {
		return _map;
	}

	void World::applyMove(Unit& unit, const Coord& to) {
		const Coord from = unit.position();
		if (unit.blocksCell()) {
			_map.clear(from);
			_map.setOccupied(to, static_cast<int32_t>(unit.id()));
		}
		unit.setPosition(to);
	}

	void World::changeUnitHp(uint32_t unitId, int32_t delta) {
		Unit* u = getUnit(unitId);
		if (u)
			u->setHp(u->hp() + delta);
	}

	void World::setUnitMarchTarget(uint32_t unitId, const Coord& target) {
		Unit* u = getUnit(unitId);
		if (u)
			u->setMarchTarget(target);
	}

	void World::clearUnitMarch(uint32_t unitId) {
		Unit* u = getUnit(unitId);
		if (u)
			u->clearMarch();
	}

	std::vector<uint32_t> World::removeDeadUnits() {
		std::vector<uint32_t> removed;
		for (const auto& uptr : _units) {
			if (!uptr || uptr->hp() > 0)
				continue;
			removed.push_back(uptr->id());
		}
		for (uint32_t id : removed)
			removeUnit(id);
		return removed;
	}

	size_t World::aliveUnitsCount() const {
		size_t count = 0;
		for (const auto& unit : _units) {
			if (unit && unit->hp() > 0)
				++count;
		}
		return count;
	}

	void World::removeUnit(uint32_t id) {
		auto it = _byId.find(id);
		if (it == _byId.end())
			return;

		const size_t idx = it->second;
		Unit* unit = _units[idx].get();
		if (!unit)
			return;
		if (unit->blocksCell())
			_map.clear(unit->position());

		_byId.erase(it);
		_units[idx].reset();
	}

	// WorldView
	WorldView::WorldView(World& world) : _world(world) {}

	const GridMap& WorldView::map() const {
		return _world.map();
	}

	std::vector<uint32_t> WorldView::neighboringUnits(const Coord& center) {
		return _world.neighboringUnits(center);
	}

	std::vector<uint32_t> WorldView::unitsInChebyshevRing(const Coord& center, int32_t minD, int32_t maxD) {
		return _world.unitsInChebyshevRing(center, minD, maxD);
	}

	bool WorldView::hasNeighbouringBlockingUnit(const Coord& coordinate) {
		return _world.hasNeighbouringBlockingUnit(coordinate);
	}

	void WorldView::applyMove(Unit& unit, const Coord& to) {
		_world.applyMove(unit, to);
	}

	void WorldView::changeHP(uint32_t unitId, int32_t delta) {
		_world.changeUnitHp(unitId, delta);
	}

	void WorldView::setMarchTarget(uint32_t unitId, const Coord& target) {
		_world.setUnitMarchTarget(unitId, target);
	}

	void WorldView::clearMarch(uint32_t unitId) {
		_world.clearUnitMarch(unitId);
	}

	std::optional<int32_t> WorldView::getUnitHp(uint32_t unitId) const {
		return _world.getUnitHp(unitId);
	}

	std::optional<Coord> WorldView::getUnitPosition(uint32_t unitId) const {
		return _world.getUnitPosition(unitId);
	}

	bool WorldView::getUnitBlocksCell(uint32_t unitId) const {
		return _world.getUnitBlocksCell(unitId);
	}
}
