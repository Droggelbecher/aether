
#ifndef _COORDINATE_H_
#define _COORDINATE_H_

#include <iostream>
#include <cstdint>
#include <vector>
#include <array>

template<typename T, int N>
class Coordinate {
	public:
		template<typename... U>
		Coordinate(U... values)
			: _values{ values... } {
		}

		Coordinate operator+(const Coordinate& other) const {
			// Wow, this is suprisingly verbose. Why is there nothing like pythons zip() in std::?
			static_assert(other.dimensions() == dimensions());
			Coordinate r;
			for(int i = 0; i < N; i++) {
				r[i] = _values[i] + other._values[i];
			}
			return r;
		}

		template<typename U>
		Coordinate operator*(const U& scalar) const {
			Coordinate r;
			for(int i = 0; i < N; i++) {
				r[i] = _values[i] * scalar;
			}
			return r;
		}

		template<typename U>
		Coordinate operator/(const U& scalar) const {
			Coordinate r;
			for(int i = 0; i < N; i++) {
				r[i] = _values[i] / scalar;
			}
			return r;
		}

		Coordinate& operator+=(const Coordinate& other) {
			for(int i = 0; i < N; i++) {
				_values[i] += other._values[i];
			}
			return *this;
		}

		Coordinate& operator-=(const Coordinate& other) {
			for(int i = 0; i < N; i++) {
				_values[i] -= other._values[i];
			}
			return *this;
		}

		T& operator[](size_t i) { return _values[i]; }

		T& x() { return _values[0]; }
		T& y() { return _values[1]; }
		T& z() { return _values[2]; }

		static constexpr size_t dimensions() { return N; }

		template<typename U>
		Coordinate<U, N> as_type() const {
			Coordinate<U, N> r;
			for(int i = 0; i < N; i++) {
				r[i] = static_cast<U>(_values[i]);
			}
			return r;
		}

	private:
		std::array<T, N> _values;
};

template<typename T, int N, typename U>
Coordinate<T, N> operator*(const U& scalar, const Coordinate<T, N>& coord) {
	Coordinate<T, N> r;
	for(int i = 0; i < coord.dimension(); i++) {
		r[i] = coord[i] * scalar;
	}
	return r;
}

using Coord2i = Coordinate<int, 2>;
using Coordi = Coord2i;
using Coord3d = Coordinate<double, 3>;
using Coord2d = Coordinate<double, 2>;

using CoordBatch2d = std::vector<Coord2d>;
using CoordBatch3d = std::vector<Coord3d>;
using CoordBatch2i = std::vector<Coord2i>;

#endif // _COORDINATE_H_
