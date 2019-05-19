
#ifndef _COORDINATE_H_
#define _COORDINATE_H_

#include <iostream>
#include <cstdint>

template<typename T_>
struct Coordinate {
	using T = T_;
	T x;
	T y;

	Coordinate() : x(0), y(0) {
	}

	Coordinate(T x, T y) : x(x), y(y) {
	}

	bool operator==(const Coordinate& other) {
		return (x == other.x) && (y == other.y);
	}

	Coordinate operator-(const Coordinate& other) {
		return {x - other.x, y - other.y};
	}
};

using Coordi = Coordinate<int>;

#endif // _COORDINATE_H_
