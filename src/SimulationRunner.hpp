#pragma once

#include <Core/World.hpp>
#include <IO/System/CommandParser.hpp>
#include <IO/System/EventLog.hpp>

#include <cstdint>
#include <istream>
#include <memory>

namespace sw {

	class SimulationRunner {

	public:
		SimulationRunner();
		void run(std::istream& stream);

	private:
		void setupParser();

		uint64_t _tick = 1;
		io::CommandParser _parser;
		EventLog _eventLog;
		std::unique_ptr<core::World> _world;
	};
}
