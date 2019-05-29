
#ifndef _COORDINATE_H_
#define _COORDINATE_H_

#include <iostream>
#include <cstdint>

#include <xtensor/xfixed.hpp>

/**
 * 2d pixel position on the screen.
 * TODO: Can we provide convenient .x() & .y() while still
 * keeping all of xtensors functionality?
 * Can we make it so it works nicely also with an array of Coordi's (i.e. a n x 2 xtensor)?
 */
using Coord2i = xt::xtensor_fixed<int16_t, xt::xshape<2> >;
using Coordi = Coord2i;
using Coord3d = xt::xtensor_fixed<double, xt::xshape<3> >;
using Coord2d = xt::xtensor_fixed<double, xt::xshape<2> >;

using CoordBatch2d = xt::xtensor<double, 2>;
using CoordBatch3d = xt::xtensor<double, 2>;
using CoordBatch2i = xt::xtensor<int16_t, 2>;

#endif // _COORDINATE_H_
