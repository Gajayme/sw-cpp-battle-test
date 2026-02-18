#pragma once

#include "Coord.hpp"

#include <cstdint>
#include <stdexcept>
#include <vector>

namespace sw::core {

	class GridMap {
	public:
		GridMap(uint32_t width, uint32_t height)
			: _width(width)
			, _height(height)
			, _cells(static_cast<size_t>(width) * static_cast<size_t>(height), kEmptyCell)
		{
			if (width == 0 || height == 0) {
				throw std::runtime_error("Map size must be positive");
			}
		}

		uint32_t width() const {
			return _width;
		}

		uint32_t height() const {
			return _height;
		}

		bool inBounds(const Coord& coordinate) const {
			return coordinate.x >= 0 && coordinate.y >= 0 && static_cast<uint32_t>(coordinate.x) < _width && static_cast<uint32_t>(coordinate.y) < _height;
		}

		bool isOccupied(const Coord& coordinate) const {
			return occupantId(coordinate) != kEmptyCell;
		}

		int32_t occupantId(const Coord& coordinate) const {
			if (!inBounds(coordinate)) {
				return kEmptyCell;
			}
			return _cells[indexOf(coordinate)];
		}

		void setOccupied(const Coord& coordinate, int32_t unitId) {
			if (!inBounds(coordinate)) {
				throw std::runtime_error("Coord out of bounds");
			}
			_cells[indexOf(coordinate)] = unitId;
		}

		void clear(const Coord& coordinate) {
			if (!inBounds(coordinate)) {
				return;
			}
			_cells[indexOf(coordinate)] = kEmptyCell;
		}

		// Константа для пустой клетки
		static constexpr int32_t kEmptyCell = -1;

	private:
		size_t indexOf(const Coord& c) const {
			return static_cast<size_t>(c.y) * static_cast<size_t>(_width) + static_cast<size_t>(c.x);
		}

		uint32_t _width{};
		uint32_t _height{};
		std::vector<int32_t> _cells;
	};
}

