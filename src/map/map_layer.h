
#ifndef _MAP_LAYER_H_
#define _MAP_LAYER_H_

#include <algorithm>
#include <vector>
#include <cassert>
#include <memory>

#include "hex_coordinate.h"

template<typename T>
class MapLayer {

		class HexIterator {
			public:
				HexIterator(Hex i, int q_max): _i(i), _q_max(q_max) {
				}

				Hex operator*() { return _i; }
				HexIterator& operator++() {
					if(_i.q() + 1 >= _q_max) {
						_i = { 0, _i.r() + 1 };
					}
					else {
						_i = { _i.q() + 1, _i.r() };
					}
					return *this;
				}
				bool operator==(const HexIterator& other) const {
					return _i == other._i;
				}
				bool operator!=(const HexIterator& other) const {
					return !(*this == other);
				}
			private:
				Hex _i;
				int _q_max;
		};

	public:
		MapLayer(Hex dimensions) : _dimensions(dimensions) {
			_cells = new T[size()];
		}

		T& operator[](Hex h) {
			return _cells[hex2index(h)];
		}

		auto begin() {
			return HexIterator({0, 0}, _dimensions.q());
		}

		auto end() {
			return HexIterator(
					{ 0, _dimensions.r() },
					_dimensions.q()
			);
		}

	private:

		size_t hex2index(Hex h) {
			assert(h.q() < _dimensions.q());
			assert(h.q() >= 0);
			assert(h.r() < _dimensions.r());
			assert(h.r() >= 0);
			return h.q() + h.r() * _dimensions.q();
		}

		size_t size() { return _dimensions.q() * _dimensions.r(); }

		Hex index2hex(size_t i) {
			int i_ = i;
			return { i_ % _dimensions.q(), static_cast<int>(i_ / _dimensions.q()) };
		}

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

