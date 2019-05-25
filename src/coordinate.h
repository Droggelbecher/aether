
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
using Coordi = xt::xtensor_fixed<int, xt::xshape<2> >;

#endif // _COORDINATE_H_
