#include <SimulationRunner.hpp>

#include <ctime>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <stdexcept>

int main(int argc, char** argv) {
	std::srand(static_cast<unsigned>(std::time(nullptr)));

	try {
		if (argc != 2) {
			throw std::runtime_error("Error: No file specified in command line argument");
		}

		std::ifstream file(argv[1]);
		if (!file) {
			throw std::runtime_error("Error: File not found - " + std::string(argv[1]));
		}

		sw::SimulationRunner runner;
		runner.run(file);

		return 0;
	} catch (const std::exception& e) {
		std::cerr << e.what() << '\n';
		return 1;
	} catch (...) {
		std::cerr << "Unknown error\n";
		return 1;
	}
}
