#include <SimulationRunner.hpp>

#include <Core/GridMap.hpp>
#include <Features/UnitFactory.hpp>
#include <IO/Events/UnitDied.hpp>
#include <IO/Commands/CreateMap.hpp>
#include <IO/Commands/March.hpp>
#include <IO/Commands/SpawnHunter.hpp>
#include <IO/Commands/SpawnSwordsman.hpp>
#include <IO/Events/MapCreated.hpp>
#include <IO/Events/MarchStarted.hpp>
#include <IO/Events/UnitSpawned.hpp>

#include <stdexcept>
#include <string>

namespace sw {

	constexpr uint64_t kMaxSimulationTicks = 10000;

	SimulationRunner::SimulationRunner() {
		setupParser();
	}

	void SimulationRunner::run(std::istream& stream) {
		_parser.parse(stream);

		if (!_world)
			throw std::runtime_error("Scenario did not create a map");

		core::WorldView worldView(*_world);

		// На всякий случай добавим ограничение на количество ходов
		// Основной цикл симуляции
		while (_world->aliveUnitsCount() > 1 && _tick < kMaxSimulationTicks) {
			++_tick;
			bool anyActed = false;
			for (const auto& uptr : _world->unitsInCreationOrder()) {
				if (!uptr)
					continue;

				core::TurnContext ctx{worldView, _eventLog, _tick};
				if (uptr->takeTurn(ctx))
					anyActed = true;
			}

			// Удаляем мертвые юниты и логируем их смерть
			// Удаляем только в конце хода. Юниты с 0 хп смогут действовать в этом ходу (по условию)
			for (uint32_t id : _world->removeDeadUnits())
				_eventLog.log(_tick, io::UnitDied{id});

			// Остановка, если никто не действовал (нет юнитов, способных действовать)
			if (!anyActed)
				break;
		}
	}

	void SimulationRunner::setupParser() {

		// Создание карты
		_parser.add<io::CreateMap>([this](io::CreateMap command) {
			_world = std::make_unique<core::World>(core::GridMap{command.width, command.height});
			_eventLog.log(_tick, io::MapCreated{command.width, command.height});
		});

		// Создание мечника
		_parser.add<io::SpawnSwordsman>([this](io::SpawnSwordsman command) {
			if (!_world) throw std::runtime_error("Map is not created yet");
			const int32_t hp = static_cast<int32_t>(command.hp);
			const int32_t strength = static_cast<int32_t>(command.strength);
			if (hp < 0) throw std::runtime_error(std::string(io::SpawnSwordsman::Name) + ": hp cannot be negative");
			if (strength < 0) throw std::runtime_error(std::string(io::SpawnSwordsman::Name) + ": strength cannot be negative");
			auto unit = features::createSwordsman(
				command.unitId,
				core::Coord{static_cast<int32_t>(command.x), static_cast<int32_t>(command.y)},
				hp,
				strength);
			_world->spawn(std::move(unit));
			_eventLog.log(_tick, io::UnitSpawned{command.unitId, "Swordsman", command.x, command.y});
		});

		// Создание охотника
		_parser.add<io::SpawnHunter>([this](io::SpawnHunter command) {
			if (!_world) throw std::runtime_error("Map is not created yet");
			const int32_t hp = static_cast<int32_t>(command.hp);
			const int32_t agility = static_cast<int32_t>(command.agility);
			const int32_t strength = static_cast<int32_t>(command.strength);
			const int32_t range = static_cast<int32_t>(command.range);
			if (hp < 0) throw std::runtime_error(std::string(io::SpawnHunter::Name) + ": hp cannot be negative");
			if (agility < 0) throw std::runtime_error(std::string(io::SpawnHunter::Name) + ": agility cannot be negative");
			if (strength < 0) throw std::runtime_error(std::string(io::SpawnHunter::Name) + ": strength cannot be negative");
			if (range < 0) throw std::runtime_error(std::string(io::SpawnHunter::Name) + ": range cannot be negative");
			auto unit = features::createHunter(
				command.unitId,
				core::Coord{static_cast<int32_t>(command.x), static_cast<int32_t>(command.y)},
				hp,
				agility,
				strength,
				range);
			_world->spawn(std::move(unit));
			_eventLog.log(_tick, io::UnitSpawned{command.unitId, "Hunter", command.x, command.y});
		});

		// Добавляем команду марша
		_parser.add<io::March>([this](io::March command) {
			if (!_world) throw std::runtime_error("Map is not created yet");
			std::optional<core::Coord> from = _world->getUnitPosition(command.unitId);
			if (!from) throw std::runtime_error("Unknown unit id in MARCH");

			const core::Coord target{static_cast<int32_t>(command.targetX), static_cast<int32_t>(command.targetY)};
			if (!_world->map().inBounds(target))
				throw std::runtime_error("MARCH target is out of map bounds");
			_world->setUnitMarchTarget(command.unitId, target);
			_eventLog.log(_tick, io::MarchStarted{
								 command.unitId,
								 static_cast<uint32_t>(from->x),
								 static_cast<uint32_t>(from->y),
								 command.targetX,
								 command.targetY,
							 });
		});
	}
}
