
#ifndef _HEX_COORDINATE_H_
#define _HEX_COORDINATE_H_

#include <ostream>
#include <cassert>
#include <limits>

// https://www.redblobgames.com/grids/hexagons/
// https://www.redblobgames.com/grids/hexagons/implementation.html#fractionalhex

template<typename T>
struct HexCoordinate {

public:
	HexCoordinate(): _q(), _r() {
	}

	HexCoordinate(T q, T r, double z=0.): _q(q), _r(r), _z(z) {
	}

	HexCoordinate(const HexCoordinate& other) = default;
	HexCoordinate& operator=(const HexCoordinate& other) {
		_q = other._q;
		_r = other._r;
		_z = other._z;
		return *this;
	}

	T q() const { return _q; }
	T r() const { return _r; }
	T s() const { return -_q - _r; }
	double z() const { return _z; }

	bool operator==(HexCoordinate other) const {
		return _q == other._q && _r == other._r && _z == other._z;
	}

	bool operator!=(HexCoordinate other) const {
		return !(*this == other);
	}

	HexCoordinate operator+(const HexCoordinate& other) const {
		return { _q + other._q, _r + other._r, _z + other._z };
	}

	template<typename U>
	HexCoordinate operator/(const U& scalar) const {
		return { _q / scalar, _r / scalar, _z / scalar };
	}

	static const HexCoordinate<T> invalid;
	
private:
	T _q;
	T _r;
	double _z;
};

using Hex = HexCoordinate<int>;
using FractionalHex = HexCoordinate<double>;

std::ostream& operator<<(std::ostream& os, const Hex& hex) {
	os << "Hex{" << hex.q() << ", " << hex.r() << ", " << hex.s() << "}";
	return os;
}

template<typename T>
const HexCoordinate<T> HexCoordinate<T>::invalid {
	std::numeric_limits<T>::has_quiet_NaN ? std::numeric_limits<T>::quiet_NaN() : std::numeric_limits<T>::lowest(),
	std::numeric_limits<T>::has_quiet_NaN ? std::numeric_limits<T>::quiet_NaN() : std::numeric_limits<T>::lowest(),
	0
};

/**
 * 2-d distance in hex fields (ignores z part of coordinates)
 */
template<typename T>
T hex_distance(const HexCoordinate<T>& a, const HexCoordinate<T>& b) {
	return (
			  abs(a.q() - b.q())
			+ abs(a.s() - b.s())
			+ abs(a.r() - b.r())
	) / 2;
}

#endif // _HEX_COORDINATE_H_
