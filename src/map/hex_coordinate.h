
#ifndef _HEX_COORDINATE_H_
#define _HEX_COORDINATE_H_

#include <cassert>

// https://www.redblobgames.com/grids/hexagons/
// https://www.redblobgames.com/grids/hexagons/implementation.html#fractionalhex

template<typename T>
struct HexCoordinate {

public:
	HexCoordinate(T q, T r): _q(q), _r(r) {
	}

	T q() const { return _q; }
	T r() const { return _r; }
	T s() const { return -_q - _r; }

	bool operator==(HexCoordinate other) const {
		return _q == other._q && _r == other._r;
	}

	bool operator!=(HexCoordinate other) const {
		return !(*this == other);
	}

private:
	const T _q;
	const T _r;
};

using Hex = HexCoordinate<int>;
using FractionalHex = HexCoordinate<double>;

#endif // _HEX_COORDINATE_H_
