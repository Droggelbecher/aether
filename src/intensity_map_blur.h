
#ifndef _INTENSITY_MAP_BLUR_H_
#define _INTENSITY_MAP_BLUR_H_

#include "intensity_map.h"
#include <cmath>


IntensityMap gaussian(const IntensityMap& im, double stddev) {
	IntensityMap r(im.size());

	Coordi sz = im.size();

	int R = 100;
	
	for(Coordi::T ix = 0; ix < sz.x; ix++) {
	for(Coordi::T iy = 0; iy < sz.y; iy++) {

		double v = 0;
		for(int dx = -R; dx < R; dx++) {
		for(int dy = -R; dy < R; dy++) {
			auto f = (1. / (2. * M_PI * stddev)) * exp(-(dx*dx + dy*dy) / (2. * stddev));
			v += f * im(ix + dx, iy + dy);
		}
		}

		r(ix, iy) = v;
	}
	}

	//r.normalize();
	return r;
}


#endif // _INTENSITY_MAP_BLUR_H_

