
#ifndef _MAP_H_
#define _MAP_H_

#include <vector>
#include <cassert>
#include <memory>

#include "hex_coordinate.h"

class Map {
	using Cell = int;

	public:
		Map(Hex dimensions) : _dimensions(dimensions) {
		}

	private:

		void resize() {
			auto sz = _dimensions.q() * _dimensions.r();
			_cells.resize(sz);
		}

		size_t hex2index(Hex h) {
			assert(h.q() < _dimensions.r());
			assert(h.q() >= 0);
			assert(h.r() < _dimensions.r());
			assert(h.r() >= 0);
			return h.q() * h.r();
		}

		Hex index2hex(size_t i) {
			assert(i < _cells.size());
			int i_ = i;
			return { i_ % _dimensions.q(), static_cast<int>(i_ / _dimensions.q()) };
		}

		Hex _dimensions;

		std::vector<Cell> _cells;
};


std::unique_ptr<Map> make_hexagon(size_t n) {
	auto map = std::make_unique<Map>(2 * n - 1);
	return map;
}

#endif // _MAP_H_

