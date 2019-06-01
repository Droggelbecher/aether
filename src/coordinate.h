
#ifndef _COORDINATE_H_
#define _COORDINATE_H_

#include <iostream>
#include <cstdint>
#include <vector>
#include <array>
#include <type_traits>

template<typename T, int N>
class Coordinate;

namespace {
	template<typename>
	struct is_coordinate: public std::false_type {};

	template<typename T, int N>
	struct is_coordinate<Coordinate<T, N>>: public std::true_type {};

}

template<typename T, int N>
class Coordinate {
	public:
		using Ordinate = T;

		template<typename... U>
		Coordinate(U... values)
			: _values{ values... } {
		}

		template<typename U>
		auto operator+(const Coordinate<U, N>& other) const {
			// Wow, this is suprisingly verbose. Why is there nothing like pythons zip() in std::?
			static_assert(other.dimensions() == dimensions());
			Coordinate<decltype(T() + U()), N> r;
			for(int i = 0; i < N; i++) {
				r[i] = _values[i] + other[i];
			}
			return r;
		}

		template<typename U>
		auto operator-(const Coordinate<U, N>& other) const {
			static_assert(other.dimensions() == dimensions());
			Coordinate<decltype(T() - U()), N> r;
			for(int i = 0; i < N; i++) {
				r[i] = _values[i] - other[i];
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
				_values[i] += other[i];
			}
			return *this;
		}

		Coordinate& operator-=(const Coordinate& other) {
			for(int i = 0; i < N; i++) {
				_values[i] -= other[i];
			}
			return *this;
		}

		T& operator[](size_t i) { return _values[i]; }
		const T& operator[](size_t i) const { return _values[i]; }

		T& x() { return _values[0]; }
		T& y() { return _values[1]; }
		T& z() { return _values[2]; }

		const T& x() const { return _values[0]; }
		const T& y() const { return _values[1]; }
		const T& z() const { return _values[2]; }

		static constexpr size_t dimensions() { return N; }

		template<typename U>
		auto as_type() const {
			if constexpr (is_coordinate<U>{}) {
				static_assert(U::dimensions() == N);
				return as_type<typename U::Ordinate>();
			}
			else {
				Coordinate<U, N> r;
				for(int i = 0; i < N; i++) {
					r[i] = static_cast<U>(_values[i]);
				}
				return r;
			}
		}

	private:
		std::array<T, N> _values;
};

template<typename T, int N, typename U>
Coordinate<T, N> operator*(const U& scalar, const Coordinate<T, N>& coord) {
	Coordinate<T, N> r;
	for(size_t i = 0; i < coord.dimensions(); i++) {
		r[i] = coord[i] * scalar;
	}
	return r;
}

template<typename T, int N>
std::ostream& operator<<(std::ostream& os, const Coordinate<T, N>& coord) {
	os << "C<" << typeid(T).name() << ">(";
	for(size_t i = 0; i < coord.dimensions(); i++) {
		os << coord[i] << " ";
	}
	os << ")";
	return os;
}

using Coord2i = Coordinate<int, 2>;
using Coordi = Coord2i;
using Coord3d = Coordinate<double, 3>;
using Coord2d = Coordinate<double, 2>;

using CoordBatch2d = std::vector<Coord2d>;
using CoordBatch3d = std::vector<Coord3d>;
using CoordBatch2i = std::vector<Coord2i>;

#endif // _COORDINATE_H_
