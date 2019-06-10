
#ifndef _MAP_LAYER_H_
#define _MAP_LAYER_H_

#include <algorithm>
#include <vector>
#include <cassert>
#include <memory>
#include <optional>

#include "hex_coordinate.h"

template<typename T>
class MapLayer {

	public:
		MapLayer(Hex dimensions, std::optional<T> default_ = std::nullopt) : _dimensions(dimensions) {
			_cells = new T[size()]();
			if(default_) {
				std::fill_n(_cells, size(), *default_);
			}
		}

		~MapLayer() {
			delete _cells;
		}

		T& operator[](size_t i) {
			return _cells[i];
		}

	private:

		size_t size() { return _dimensions.q() * _dimensions.r(); }

		const Hex _dimensions;
		T *_cells;
};


template<typename T>
std::unique_ptr<MapLayer<T> > make_hexagon(int n, const T& v) {
	using namespace std;

	int N = 2*n - 1;
	auto map = make_unique<MapLayer<T> >(Hex {N, N});
	for(int r = 0; r < N; r++) {
		for(int q = 0; q < N; q++) {
			if(q < (n - r - 1) || q >= (N + n - r - 1)) {
				(*map)[{ q, r }] = T();
			}
			else {
				(*map)[{ q, r }] = v;
			}
		}
	}

	return map;
}

#endif // _MAP_H_

