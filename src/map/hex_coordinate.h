
#ifndef _HEX_COORDINATE_H_
#define _HEX_COORDINATE_H_

#include <cassert>

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

private:
	T _q;
	T _r;
	double _z;
};

using Hex = HexCoordinate<int>;
using FractionalHex = HexCoordinate<double>;

#endif // _HEX_COORDINATE_H_
